// Copyright 2017 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include "tink/core/key_manager_impl.h"
#include "tink/keyset_handle.h"

#include "gtest/gtest.h"
#include "tink/aead/aead_key_templates.h"
#include "tink/aead/aead_wrapper.h"
#include "tink/aead/aes_gcm_key_manager.h"
#include "tink/aead_key_templates.h"
#include "tink/binary_keyset_reader.h"
#include "tink/cleartext_keyset_handle.h"
#include "tink/config/tink_config.h"
#include "tink/json_keyset_reader.h"
#include "tink/json_keyset_writer.h"
#include "tink/signature/ecdsa_sign_key_manager.h"
#include "tink/signature/signature_key_templates.h"
#include "tink/util/protobuf_helper.h"
#include "tink/util/test_keyset_handle.h"
#include "tink/util/test_matchers.h"
#include "tink/util/test_util.h"
#include "proto/tink.pb.h"

namespace crypto {
namespace tink {

using crypto::tink::TestKeysetHandle;
using crypto::tink::test::AddKeyData;
using crypto::tink::test::AddLegacyKey;
using crypto::tink::test::AddRawKey;
using crypto::tink::test::AddTinkKey;
using crypto::tink::test::DummyAead;
using crypto::tink::test::IsOk;
using crypto::tink::test::StatusIs;
using google::crypto::tink::EcdsaKeyFormat;
using google::crypto::tink::EncryptedKeyset;
using google::crypto::tink::KeyData;
using google::crypto::tink::Keyset;
using google::crypto::tink::KeyStatusType;
using google::crypto::tink::KeyTemplate;
using google::crypto::tink::OutputPrefixType;

namespace {

class KeysetHandleTest : public ::testing::Test {
 protected:
  void SetUp() override {
    auto status = TinkConfig::Register();
    ASSERT_TRUE(status.ok()) << status;
  }
};

TEST_F(KeysetHandleTest, ReadEncryptedKeysetBinary) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::SYMMETRIC, &keyset);
  AddRawKey("some other key type", 711, key, KeyStatusType::ENABLED,
            KeyData::SYMMETRIC, &keyset);
  keyset.set_primary_key_id(42);

  {  // Good encrypted keyset.
    DummyAead aead("dummy aead 42");
    std::string keyset_ciphertext =
        aead.Encrypt(keyset.SerializeAsString(), /* associated_data= */ "")
            .ValueOrDie();
    EncryptedKeyset encrypted_keyset;
    encrypted_keyset.set_encrypted_keyset(keyset_ciphertext);
    auto reader = std::move(BinaryKeysetReader::New(
        encrypted_keyset.SerializeAsString()).ValueOrDie());
    auto result = KeysetHandle::Read(std::move(reader), aead);
    EXPECT_TRUE(result.ok()) << result.status();
    auto handle = std::move(result.ValueOrDie());
    EXPECT_EQ(keyset.SerializeAsString(),
              TestKeysetHandle::GetKeyset(*handle).SerializeAsString());
  }

  {  // AEAD does not match the ciphertext
    DummyAead aead("dummy aead 42");
    std::string keyset_ciphertext =
        aead.Encrypt(keyset.SerializeAsString(), /* associated_data= */ "")
            .ValueOrDie();
    EncryptedKeyset encrypted_keyset;
    encrypted_keyset.set_encrypted_keyset(keyset_ciphertext);
    auto reader = std::move(BinaryKeysetReader::New(
        encrypted_keyset.SerializeAsString()).ValueOrDie());
    DummyAead wrong_aead("wrong aead");
    auto result = KeysetHandle::Read(std::move(reader), wrong_aead);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(util::error::INVALID_ARGUMENT, result.status().error_code());
  }

  {  // Ciphertext does not contain actual keyset.
    DummyAead aead("dummy aead 42");
    std::string keyset_ciphertext =
        aead.Encrypt("not a serialized keyset", /* associated_data= */ "")
            .ValueOrDie();
    EncryptedKeyset encrypted_keyset;
    encrypted_keyset.set_encrypted_keyset(keyset_ciphertext);
    auto reader = std::move(BinaryKeysetReader::New(
        encrypted_keyset.SerializeAsString()).ValueOrDie());
    auto result = KeysetHandle::Read(std::move(reader), aead);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(util::error::INVALID_ARGUMENT, result.status().error_code());
  }

  {  // Wrong ciphertext of encrypted keyset.
    DummyAead aead("dummy aead 42");
    std::string keyset_ciphertext = "totally wrong ciphertext";
    EncryptedKeyset encrypted_keyset;
    encrypted_keyset.set_encrypted_keyset(keyset_ciphertext);
    auto reader = std::move(BinaryKeysetReader::New(
        encrypted_keyset.SerializeAsString()).ValueOrDie());
    auto result = KeysetHandle::Read(std::move(reader), aead);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(util::error::INVALID_ARGUMENT, result.status().error_code());
  }
}

TEST_F(KeysetHandleTest, ReadEncryptedKeysetJson) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::SYMMETRIC, &keyset);
  AddRawKey("some other key type", 711, key, KeyStatusType::ENABLED,
            KeyData::SYMMETRIC, &keyset);
  keyset.set_primary_key_id(42);

  {  // Good encrypted keyset.
    DummyAead aead("dummy aead 42");
    std::string keyset_ciphertext =
        aead.Encrypt(keyset.SerializeAsString(), /* associated_data= */ "")
            .ValueOrDie();
    EncryptedKeyset encrypted_keyset;
    encrypted_keyset.set_encrypted_keyset(keyset_ciphertext);
    auto* keyset_info = encrypted_keyset.mutable_keyset_info();
    keyset_info->set_primary_key_id(42);
    auto* key_info = keyset_info->add_key_info();
    key_info->set_key_id(42);
    key_info->set_type_url("dummy key type");
    key_info->set_output_prefix_type(OutputPrefixType::TINK);
    key_info->set_status(KeyStatusType::ENABLED);
    std::stringbuf buffer;
    std::unique_ptr<std::ostream> destination_stream(new std::ostream(&buffer));
    auto writer_result = JsonKeysetWriter::New(std::move(destination_stream));
    ASSERT_TRUE(writer_result.ok()) << writer_result.status();
    auto status = writer_result.ValueOrDie()->Write(encrypted_keyset);
    EXPECT_TRUE(status.ok()) << status;
    std::string json_serialized_encrypted_keyset = buffer.str();
    EXPECT_TRUE(status.ok()) << status;
    auto reader = std::move(JsonKeysetReader::New(
        json_serialized_encrypted_keyset).ValueOrDie());
    auto result = KeysetHandle::Read(std::move(reader), aead);
    EXPECT_TRUE(result.ok()) << result.status();
    auto handle = std::move(result.ValueOrDie());
    EXPECT_EQ(keyset.SerializeAsString(),
              TestKeysetHandle::GetKeyset(*handle).SerializeAsString());
  }

  {  // AEAD does not match the ciphertext
    DummyAead aead("dummy aead 42");
    std::string keyset_ciphertext =
        aead.Encrypt(keyset.SerializeAsString(), /* associated_data= */ "")
            .ValueOrDie();
    EncryptedKeyset encrypted_keyset;
    encrypted_keyset.set_encrypted_keyset(keyset_ciphertext);
    auto reader = std::move(JsonKeysetReader::New(
        encrypted_keyset.SerializeAsString()).ValueOrDie());
    DummyAead wrong_aead("wrong aead");
    auto result = KeysetHandle::Read(std::move(reader), wrong_aead);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(util::error::INVALID_ARGUMENT, result.status().error_code());
  }

  {  // Ciphertext does not contain actual keyset.
    DummyAead aead("dummy aead 42");
    std::string keyset_ciphertext =
        aead.Encrypt("not a serialized keyset", /* associated_data= */ "")
            .ValueOrDie();
    EncryptedKeyset encrypted_keyset;
    encrypted_keyset.set_encrypted_keyset(keyset_ciphertext);
    auto reader = std::move(JsonKeysetReader::New(
        encrypted_keyset.SerializeAsString()).ValueOrDie());
    auto result = KeysetHandle::Read(std::move(reader), aead);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(util::error::INVALID_ARGUMENT, result.status().error_code());
  }

  {  // Wrong ciphertext of encrypted keyset.
    DummyAead aead("dummy aead 42");
    std::string keyset_ciphertext = "totally wrong ciphertext";
    EncryptedKeyset encrypted_keyset;
    encrypted_keyset.set_encrypted_keyset(keyset_ciphertext);
    auto reader = std::move(JsonKeysetReader::New(
        encrypted_keyset.SerializeAsString()).ValueOrDie());
    auto result = KeysetHandle::Read(std::move(reader), aead);
    EXPECT_FALSE(result.ok());
    EXPECT_EQ(util::error::INVALID_ARGUMENT, result.status().error_code());
  }
}

TEST_F(KeysetHandleTest, WriteEncryptedKeyset_Json) {
  // Prepare a valid keyset handle
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::SYMMETRIC, &keyset);
  AddRawKey("some other key type", 711, key, KeyStatusType::ENABLED,
            KeyData::SYMMETRIC, &keyset);
  keyset.set_primary_key_id(42);
  auto reader = std::move(
      BinaryKeysetReader::New(keyset.SerializeAsString()).ValueOrDie());
  auto keyset_handle = std::move(
      CleartextKeysetHandle::Read(std::move(reader)).ValueOrDie());

  // Prepare a keyset writer.
  DummyAead aead("dummy aead 42");
  std::stringbuf buffer;
  std::unique_ptr<std::ostream> destination_stream(new std::ostream(&buffer));
  auto writer = std::move(
      JsonKeysetWriter::New(std::move(destination_stream)).ValueOrDie());

  // Write the keyset handle and check the result.
  auto status = keyset_handle->Write(writer.get(), aead);
  EXPECT_TRUE(status.ok()) << status;
  auto reader_result = JsonKeysetReader::New(buffer.str());
  EXPECT_TRUE(reader_result.ok()) << reader_result.status();
  auto read_encrypted_result = reader_result.ValueOrDie()->ReadEncrypted();
  EXPECT_TRUE(read_encrypted_result.ok()) << read_encrypted_result.status();
  auto encrypted_keyset = std::move(read_encrypted_result.ValueOrDie());
  auto decrypt_result = aead.Decrypt(encrypted_keyset->encrypted_keyset(),
                                     /* associated_data= */ "");
  EXPECT_TRUE(decrypt_result.status().ok()) << decrypt_result.status();
  auto decrypted = decrypt_result.ValueOrDie();
  EXPECT_EQ(decrypted, keyset.SerializeAsString());

  // Try writing to a null-writer.
  status = keyset_handle->Write(nullptr, aead);
  EXPECT_FALSE(status.ok());
  EXPECT_EQ(util::error::INVALID_ARGUMENT, status.error_code());
}

TEST_F(KeysetHandleTest, GenerateNewKeysetHandle) {
  const google::crypto::tink::KeyTemplate* key_templates[] = {
    &AeadKeyTemplates::Aes128Gcm(),
    &AeadKeyTemplates::Aes256Gcm(),
    &AeadKeyTemplates::Aes128CtrHmacSha256(),
    &AeadKeyTemplates::Aes256CtrHmacSha256(),
  };
  for (auto templ : key_templates) {
    auto handle_result = KeysetHandle::GenerateNew(*templ);
    EXPECT_TRUE(handle_result.ok())
        << "Failed for template:\n " << templ->SerializeAsString()
        << "\n with status: "<< handle_result.status();
  }
}

TEST_F(KeysetHandleTest, GenerateNewKeysetHandleErrors) {
  KeyTemplate templ;
  templ.set_type_url("type.googleapis.com/some.unknown.KeyType");

  auto handle_result = KeysetHandle::GenerateNew(templ);
  EXPECT_FALSE(handle_result.ok());
  EXPECT_EQ(util::error::NOT_FOUND, handle_result.status().error_code());
}


void CompareKeyMetadata(const Keyset::Key& expected,
                        const Keyset::Key& actual) {
  EXPECT_EQ(expected.status(), actual.status());
  EXPECT_EQ(expected.key_id(), actual.key_id());
  EXPECT_EQ(expected.output_prefix_type(), actual.output_prefix_type());
}

TEST_F(KeysetHandleTest, GetPublicKeysetHandle) {
  { // A keyset with a single key.
    auto handle_result = KeysetHandle::GenerateNew(
        SignatureKeyTemplates::EcdsaP256());
    ASSERT_TRUE(handle_result.ok()) << handle_result.status();
    auto handle = std::move(handle_result.ValueOrDie());
    auto public_handle_result = handle->GetPublicKeysetHandle();
    ASSERT_TRUE(public_handle_result.ok()) << public_handle_result.status();
    auto keyset = TestKeysetHandle::GetKeyset(*handle);
    auto public_keyset =
        TestKeysetHandle::GetKeyset(*(public_handle_result.ValueOrDie()));
    EXPECT_EQ(keyset.primary_key_id(), public_keyset.primary_key_id());
    EXPECT_EQ(keyset.key_size(), public_keyset.key_size());
    CompareKeyMetadata(keyset.key(0), public_keyset.key(0));
    EXPECT_EQ(KeyData::ASYMMETRIC_PUBLIC,
              public_keyset.key(0).key_data().key_material_type());
  }
  { // A keyset with multiple keys.
    EcdsaSignKeyManager key_manager;
    Keyset keyset;
    int key_count = 3;

    EcdsaKeyFormat key_format;
    ASSERT_TRUE(
        key_format.ParseFromString(SignatureKeyTemplates::EcdsaP256().value()));
    AddTinkKey(EcdsaSignKeyManager().get_key_type(),
               /* key_id= */ 623628,
               key_manager.CreateKey(key_format).ValueOrDie(),
               KeyStatusType::ENABLED, KeyData::ASYMMETRIC_PRIVATE, &keyset);
    ASSERT_TRUE(
        key_format.ParseFromString(SignatureKeyTemplates::EcdsaP384().value()));
    AddLegacyKey(EcdsaSignKeyManager().get_key_type(),
                 /* key_id= */ 36285,
                 key_manager.CreateKey(key_format).ValueOrDie(),
                 KeyStatusType::DISABLED, KeyData::ASYMMETRIC_PRIVATE, &keyset);
    ASSERT_TRUE(
        key_format.ParseFromString(SignatureKeyTemplates::EcdsaP384().value()));
    AddRawKey(EcdsaSignKeyManager().get_key_type(),
              /* key_id= */ 42, key_manager.CreateKey(key_format).ValueOrDie(),
              KeyStatusType::ENABLED, KeyData::ASYMMETRIC_PRIVATE, &keyset);
    keyset.set_primary_key_id(42);
    auto handle = TestKeysetHandle::GetKeysetHandle(keyset);
    auto public_handle_result = handle->GetPublicKeysetHandle();
    ASSERT_TRUE(public_handle_result.ok()) << public_handle_result.status();
    auto public_keyset =
        TestKeysetHandle::GetKeyset(*(public_handle_result.ValueOrDie()));
    EXPECT_EQ(keyset.primary_key_id(), public_keyset.primary_key_id());
    EXPECT_EQ(keyset.key_size(), public_keyset.key_size());
    for (int i = 0; i < key_count; i++) {
      CompareKeyMetadata(keyset.key(i), public_keyset.key(i));
      EXPECT_EQ(KeyData::ASYMMETRIC_PUBLIC,
                public_keyset.key(i).key_data().key_material_type());
    }
  }
}


TEST_F(KeysetHandleTest, GetPublicKeysetHandleErrors) {
  { // A keyset with a single key.
    auto handle_result = KeysetHandle::GenerateNew(
        AeadKeyTemplates::Aes128Eax());
    ASSERT_TRUE(handle_result.ok()) << handle_result.status();
    auto handle = std::move(handle_result.ValueOrDie());
    auto public_handle_result = handle->GetPublicKeysetHandle();
    ASSERT_FALSE(public_handle_result.ok());
    EXPECT_PRED_FORMAT2(testing::IsSubstring, "ASYMMETRIC_PRIVATE",
                        public_handle_result.status().error_message());
  }
  { // A keyset with multiple keys.
    Keyset keyset;

    EcdsaKeyFormat ecdsa_key_format;
    ASSERT_TRUE(ecdsa_key_format.ParseFromString(
        SignatureKeyTemplates::EcdsaP256().value()));
    google::crypto::tink::AesGcmKeyFormat aead_key_format;
    aead_key_format.set_key_size(16);
    AddTinkKey(EcdsaSignKeyManager().get_key_type(),
               /* key_id= */ 623628,
               EcdsaSignKeyManager().CreateKey(ecdsa_key_format).ValueOrDie(),
               KeyStatusType::ENABLED, KeyData::ASYMMETRIC_PRIVATE, &keyset);
    AddLegacyKey(AesGcmKeyManager().get_key_type(),
                 /* key_id= */ 42,
                 AesGcmKeyManager().CreateKey(aead_key_format).ValueOrDie(),
                 KeyStatusType::ENABLED,
                 KeyData::ASYMMETRIC_PRIVATE,  // Intentionally wrong setting.
                 &keyset);
    keyset.set_primary_key_id(42);
    auto handle = TestKeysetHandle::GetKeysetHandle(keyset);
    auto public_handle_result = handle->GetPublicKeysetHandle();
    ASSERT_FALSE(public_handle_result.ok());
    EXPECT_PRED_FORMAT2(testing::IsSubstring, "PrivateKeyFactory",
                        public_handle_result.status().error_message());
  }
}

TEST_F(KeysetHandleTest, GetPrimitive) {
  Keyset keyset;
  KeyData key_data_0 =
      *Registry::NewKeyData(AeadKeyTemplates::Aes128Gcm()).ValueOrDie();
  AddKeyData(key_data_0, /*key_id=*/0,
             google::crypto::tink::OutputPrefixType::TINK,
             KeyStatusType::ENABLED, &keyset);
  KeyData key_data_1 =
      *Registry::NewKeyData(AeadKeyTemplates::Aes256Gcm()).ValueOrDie();
  AddKeyData(key_data_1, /*key_id=*/1,
             google::crypto::tink::OutputPrefixType::TINK,
             KeyStatusType::ENABLED, &keyset);
  KeyData key_data_2 =
      *Registry::NewKeyData(AeadKeyTemplates::Aes256Gcm()).ValueOrDie();
  AddKeyData(key_data_2, /*key_id=*/2,
             google::crypto::tink::OutputPrefixType::RAW,
             KeyStatusType::ENABLED, &keyset);
  keyset.set_primary_key_id(1);
  std::unique_ptr<KeysetHandle> keyset_handle =
      TestKeysetHandle::GetKeysetHandle(keyset);

  // Check that encryption with the primary can be decrypted with key_data_1.
  auto aead_result = keyset_handle->GetPrimitive<Aead>();
  ASSERT_TRUE(aead_result.ok()) << aead_result.status();
  std::unique_ptr<Aead> aead = std::move(aead_result.ValueOrDie());

  std::string plaintext = "plaintext";
  std::string aad = "aad";
  std::string encryption = aead->Encrypt(plaintext, aad).ValueOrDie();
  EXPECT_EQ(aead->Decrypt(encryption, aad).ValueOrDie(), plaintext);

  std::unique_ptr<Aead> raw_aead =
      Registry::GetPrimitive<Aead>(key_data_2).ValueOrDie();
  EXPECT_FALSE(raw_aead->Decrypt(encryption, aad).ok());

  std::string raw_encryption = raw_aead->Encrypt(plaintext, aad).ValueOrDie();
  EXPECT_EQ(aead->Decrypt(raw_encryption, aad).ValueOrDie(), plaintext);
}

// Tests that GetPrimitive(nullptr) fails with a non-ok status.
TEST_F(KeysetHandleTest, GetPrimitiveNullptrKeyManager) {
  Keyset keyset;
  AddKeyData(*Registry::NewKeyData(AeadKeyTemplates::Aes128Gcm()).ValueOrDie(),
             /*key_id=*/0, google::crypto::tink::OutputPrefixType::TINK,
             KeyStatusType::ENABLED, &keyset);
  keyset.set_primary_key_id(0);
  std::unique_ptr<KeysetHandle> keyset_handle =
      TestKeysetHandle::GetKeysetHandle(keyset);
  ASSERT_THAT(keyset_handle->GetPrimitive<Aead>(nullptr).status(),
              test::StatusIs(util::error::INVALID_ARGUMENT));
}

// Test creating with custom key manager. For this, we reset the registry before
// asking for the primitive.
TEST_F(KeysetHandleTest, GetPrimitiveCustomKeyManager) {
  auto handle_result = KeysetHandle::GenerateNew(AeadKeyTemplates::Aes128Gcm());
  ASSERT_TRUE(handle_result.ok()) << handle_result.status();
  std::unique_ptr<KeysetHandle> handle = std::move(handle_result.ValueOrDie());
  Registry::Reset();
  ASSERT_TRUE(
      Registry::RegisterPrimitiveWrapper(absl::make_unique<AeadWrapper>())
          .ok());
  // Without custom key manager it now fails.
  ASSERT_FALSE(handle->GetPrimitive<Aead>().ok());
  AesGcmKeyManager key_type_manager;
  std::unique_ptr<KeyManager<Aead>> key_manager =
      crypto::tink::internal::MakeKeyManager<Aead>(&key_type_manager);
  // With custom key manager it works ok.
  ASSERT_TRUE(handle->GetPrimitive<Aead>(key_manager.get()).ok());
}

// Compile time check: ensures that the KeysetHandle can be copied.
TEST_F(KeysetHandleTest, Copiable) {
  auto handle_result = KeysetHandle::GenerateNew(AeadKeyTemplates::Aes128Eax());
  ASSERT_TRUE(handle_result.ok()) << handle_result.status();
  std::unique_ptr<KeysetHandle> handle = std::move(handle_result.ValueOrDie());
  KeysetHandle handle_copy = *handle;
}

TEST_F(KeysetHandleTest, ReadNoSecret) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::ASYMMETRIC_PUBLIC, &keyset);
  AddRawKey("some other key type", 711, key, KeyStatusType::ENABLED,
            KeyData::REMOTE, &keyset);
  keyset.set_primary_key_id(42);
  auto handle_result = KeysetHandle::ReadNoSecret(keyset.SerializeAsString());
  ASSERT_THAT(handle_result.status(), IsOk());
  std::unique_ptr<KeysetHandle>& keyset_handle = handle_result.ValueOrDie();

  const Keyset& result = CleartextKeysetHandle::GetKeyset(*keyset_handle);
  // We check that result equals keyset. For lack of a better method we do this
  // by hand.
  EXPECT_EQ(result.primary_key_id(), keyset.primary_key_id());
  ASSERT_EQ(result.key_size(), keyset.key_size());
  ASSERT_EQ(result.key(0).key_id(), keyset.key(0).key_id());
  ASSERT_EQ(result.key(1).key_id(), keyset.key(1).key_id());
}

TEST_F(KeysetHandleTest, ReadNoSecretFailForTypeUnknown) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::UNKNOWN_KEYMATERIAL, &keyset);
  keyset.set_primary_key_id(42);
  auto result = KeysetHandle::ReadNoSecret(keyset.SerializeAsString());
  EXPECT_THAT(result.status(), StatusIs(util::error::FAILED_PRECONDITION));
}

TEST_F(KeysetHandleTest, ReadNoSecretFailForTypeSymmetric) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::SYMMETRIC, &keyset);
  keyset.set_primary_key_id(42);
  auto result = KeysetHandle::ReadNoSecret(keyset.SerializeAsString());
  EXPECT_THAT(result.status(), StatusIs(util::error::FAILED_PRECONDITION));
}

TEST_F(KeysetHandleTest, ReadNoSecretFailForTypeAssymmetricPrivate) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::ASYMMETRIC_PRIVATE, &keyset);
  keyset.set_primary_key_id(42);
  auto result = KeysetHandle::ReadNoSecret(keyset.SerializeAsString());
  EXPECT_THAT(result.status(), StatusIs(util::error::FAILED_PRECONDITION));
}

TEST_F(KeysetHandleTest, ReadNoSecretFailForHidden) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::ASYMMETRIC_PUBLIC, &keyset);
  for (int i = 0; i < 10; ++i) {
    AddTinkKey(absl::StrCat("more key type", i), i, key, KeyStatusType::ENABLED,
               KeyData::ASYMMETRIC_PUBLIC, &keyset);
  }
  AddRawKey("some other key type", 10, key, KeyStatusType::ENABLED,
            KeyData::ASYMMETRIC_PRIVATE, &keyset);
  for (int i = 0; i < 10; ++i) {
    AddRawKey(absl::StrCat("more key type", i + 100), i + 100, key,
              KeyStatusType::ENABLED, KeyData::ASYMMETRIC_PUBLIC, &keyset);
  }

  keyset.set_primary_key_id(42);
  auto result = KeysetHandle::ReadNoSecret(keyset.SerializeAsString());
  EXPECT_THAT(result.status(), StatusIs(util::error::FAILED_PRECONDITION));
}

TEST_F(KeysetHandleTest, ReadNoSecretFailForInvalidString) {
  auto result = KeysetHandle::ReadNoSecret("bad serialized keyset");
  EXPECT_FALSE(result.ok());
  EXPECT_EQ(util::error::INVALID_ARGUMENT, result.status().error_code());
}

TEST_F(KeysetHandleTest, WriteNoSecret) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::ASYMMETRIC_PUBLIC, &keyset);
  AddRawKey("some other key type", 711, key, KeyStatusType::ENABLED,
            KeyData::REMOTE, &keyset);
  keyset.set_primary_key_id(42);

  auto handle = TestKeysetHandle::GetKeysetHandle(keyset);

  std::stringbuf buffer;
  std::unique_ptr<std::ostream> destination_stream(new std::ostream(&buffer));
  auto writer =
      test::DummyKeysetWriter::New(std::move(destination_stream)).ValueOrDie();
  auto result = handle->WriteNoSecret(writer.get());
  EXPECT_TRUE(result.ok());
}

TEST_F(KeysetHandleTest, WriteNoSecretFailForTypeUnknown) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::UNKNOWN_KEYMATERIAL, &keyset);
  keyset.set_primary_key_id(42);

  auto handle = TestKeysetHandle::GetKeysetHandle(keyset);

  std::stringbuf buffer;
  std::unique_ptr<std::ostream> destination_stream(new std::ostream(&buffer));
  auto writer =
      test::DummyKeysetWriter::New(std::move(destination_stream)).ValueOrDie();
  auto result = handle->WriteNoSecret(writer.get());
  EXPECT_FALSE(result.ok());
}

TEST_F(KeysetHandleTest, WriteNoSecretFailForTypeSymmetric) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::SYMMETRIC, &keyset);
  keyset.set_primary_key_id(42);

  auto handle = TestKeysetHandle::GetKeysetHandle(keyset);

  std::stringbuf buffer;
  std::unique_ptr<std::ostream> destination_stream(new std::ostream(&buffer));
  auto writer =
      test::DummyKeysetWriter::New(std::move(destination_stream)).ValueOrDie();
  auto result = handle->WriteNoSecret(writer.get());
  EXPECT_FALSE(result.ok());
}

TEST_F(KeysetHandleTest, WriteNoSecretFailForTypeAssymmetricPrivate) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::ASYMMETRIC_PRIVATE, &keyset);
  keyset.set_primary_key_id(42);

  auto handle = TestKeysetHandle::GetKeysetHandle(keyset);

  std::stringbuf buffer;
  std::unique_ptr<std::ostream> destination_stream(new std::ostream(&buffer));
  auto writer =
      test::DummyKeysetWriter::New(std::move(destination_stream)).ValueOrDie();
  auto result = handle->WriteNoSecret(writer.get());
  EXPECT_FALSE(result.ok());
}

TEST_F(KeysetHandleTest, WriteNoSecretFailForHidden) {
  Keyset keyset;
  Keyset::Key key;
  AddTinkKey("some key type", 42, key, KeyStatusType::ENABLED,
             KeyData::ASYMMETRIC_PUBLIC, &keyset);
  for (int i = 0; i < 10; ++i) {
    AddTinkKey(absl::StrCat("more key type", i), i, key, KeyStatusType::ENABLED,
               KeyData::ASYMMETRIC_PUBLIC, &keyset);
  }
  AddRawKey("some other key type", 10, key, KeyStatusType::ENABLED,
            KeyData::ASYMMETRIC_PRIVATE, &keyset);
  for (int i = 0; i < 10; ++i) {
    AddRawKey(absl::StrCat("more key type", i + 100), i + 100, key,
              KeyStatusType::ENABLED, KeyData::ASYMMETRIC_PUBLIC, &keyset);
  }

  keyset.set_primary_key_id(42);

  auto handle = TestKeysetHandle::GetKeysetHandle(keyset);

  std::stringbuf buffer;
  std::unique_ptr<std::ostream> destination_stream(new std::ostream(&buffer));
  auto writer =
      test::DummyKeysetWriter::New(std::move(destination_stream)).ValueOrDie();
  auto result = handle->WriteNoSecret(writer.get());
  EXPECT_FALSE(result.ok());
}

}  // namespace
}  // namespace tink
}  // namespace crypto
