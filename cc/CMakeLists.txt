add_subdirectory(aead)
add_subdirectory(config)
add_subdirectory(daead)
add_subdirectory(hybrid)
add_subdirectory(mac)
add_subdirectory(signature)
add_subdirectory(streamingaead)
add_subdirectory(subtle)
add_subdirectory(util)

tink_module(core)

# public libraries

set(TINK_VERSION_H "${TINK_GENFILE_DIR}/tink/version.h")

set(TINK_PUBLIC_APIS
  aead.h
  aead_config.h
  aead_factory.h
  aead_key_templates.h
  binary_keyset_reader.h
  binary_keyset_writer.h
  catalogue.h
  config.h
  deterministic_aead.h
  deterministic_aead_config.h
  deterministic_aead_factory.h
  deterministic_aead_key_templates.h
  hybrid_config.h
  hybrid_decrypt.h
  hybrid_decrypt_factory.h
  hybrid_encrypt.h
  hybrid_encrypt_factory.h
  hybrid_key_templates.h
  input_stream.h
  json_keyset_reader.h
  json_keyset_writer.h
  key_manager.h
  keyset_handle.h
  keyset_manager.h
  keyset_reader.h
  keyset_writer.h
  kms_client.h
  mac.h
  mac_config.h
  mac_factory.h
  mac_key_templates.h
  output_stream_with_result.h
  output_stream.h
  public_key_sign.h
  public_key_sign_factory.h
  public_key_verify.h
  public_key_verify_factory.h
  random_access_stream.h
  registry.h
  signature_config.h
  signature_key_templates.h
  streaming_aead.h
  streaming_aead_config.h
  streaming_aead_key_templates.h
  streaming_mac.h
  tink_config.h
  "${TINK_VERSION_H}"
)

set(TINK_PUBLIC_API_DEPS
  tink::core::aead
  tink::core::binary_keyset_reader
  tink::core::binary_keyset_writer
  tink::core::cleartext_keyset_handle
  tink::core::deterministic_aead
  tink::core::hybrid_decrypt
  tink::core::hybrid_encrypt
  tink::core::input_stream
  tink::core::json_keyset_reader
  tink::core::json_keyset_writer
  tink::core::key_manager
  tink::core::keyset_handle
  tink::core::keyset_manager
  tink::core::keyset_reader
  tink::core::keyset_writer
  tink::core::kms_client
  tink::core::output_stream_with_result
  tink::core::output_stream
  tink::core::public_key_sign
  tink::core::public_key_verify
  tink::core::mac
  tink::core::primitive_set
  tink::core::random_access_stream
  tink::core::registry
  tink::core::registry_impl
  tink::core::streaming_aead
  tink::core::streaming_mac
  tink::core::version
  tink::aead::aead_config
  tink::aead::aead_factory
  tink::aead::aead_key_templates
  tink::config::tink_config
  tink::daead::deterministic_aead_config
  tink::daead::deterministic_aead_factory
  tink::daead::deterministic_aead_key_templates
  tink::hybrid::hybrid_config
  tink::hybrid::hybrid_decrypt_factory
  tink::hybrid::hybrid_encrypt_factory
  tink::hybrid::hybrid_key_templates
  tink::mac::mac_config
  tink::mac::mac_factory
  tink::mac::mac_key_templates
  tink::signature::public_key_sign_factory
  tink::signature::public_key_verify_factory
  tink::signature::signature_config
  tink::signature::signature_key_templates
  tink::streamingaead::streaming_aead_config
  tink::streamingaead::streaming_aead_key_templates
  tink::util::buffer
  tink::util::errors
  tink::util::protobuf_helper
  tink::util::status
  tink::util::statusor
  tink::util::validation
  absl::base
  absl::memory
  absl::strings
  absl::synchronization
)

tink_cc_library(
  NAME cc
  SRCS ${TINK_PUBLIC_APIS}
  DEPS ${TINK_PUBLIC_API_DEPS}
  PUBLIC
)

add_library(tink::static ALIAS tink_core_cc)

tink_cc_library(
  NAME input_stream
  SRCS input_stream.h
  DEPS
    tink::util::status
    tink::util::statusor
)

tink_cc_library(
  NAME output_stream
  SRCS output_stream.h
  DEPS
    tink::util::status
    tink::util::statusor
)

tink_cc_library(
  NAME random_access_stream
  SRCS random_access_stream.h
  DEPS
    tink::util::buffer
    tink::util::status
    tink::util::statusor
)

tink_cc_library(
  NAME output_stream_with_result
  SRCS output_stream_with_result.h
  DEPS
    tink::core::output_stream_with_result
    tink::util::status
    tink::util::statusor
)

tink_cc_library(
  NAME aead
  SRCS aead.h
  DEPS
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME deterministic_aead
  SRCS deterministic_aead.h
  DEPS
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME streaming_aead
  SRCS streaming_aead.h
  DEPS
    tink::core::input_stream
    tink::core::output_stream
    tink::core::random_access_stream
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME streaming_mac
  SRCS streaming_mac.h
  DEPS
    tink::core::output_stream_with_result
    tink::util::status
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME hybrid_decrypt
  SRCS hybrid_decrypt.h
  DEPS
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME hybrid_encrypt
  SRCS hybrid_encrypt.h
  DEPS
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME mac
  SRCS mac.h
  DEPS
    tink::util::status
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME public_key_sign
  SRCS public_key_sign.h
  DEPS
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME public_key_verify
  SRCS public_key_verify.h
  DEPS
    tink::util::status
    absl::strings
)

tink_cc_library(
  NAME keyset_reader
  SRCS keyset_reader.h
  DEPS
    tink::util::statusor
    tink::proto::tink_cc_proto
)

tink_cc_library(
  NAME keyset_writer
  SRCS keyset_writer.h
  DEPS
    tink::util::status
    tink::proto::tink_cc_proto
)

tink_cc_library(
  NAME binary_keyset_reader
  SRCS
    core/binary_keyset_reader.cc
    binary_keyset_reader.h
  DEPS
    tink::core::keyset_reader
    tink::util::errors
    tink::util::statusor
    tink::proto::tink_cc_proto
    absl::memory
    absl::strings
    protobuf::libprotobuf-lite
)

tink_cc_library(
  NAME binary_keyset_writer
  SRCS
    core/binary_keyset_writer.cc
    binary_keyset_writer.h
  DEPS
    tink::core::keyset_writer
    tink::util::errors
    tink::util::protobuf_helper
    tink::util::status
    tink::util::statusor
    tink::proto::tink_cc_proto
    absl::strings
)

tink_cc_library(
  NAME json_keyset_reader
  SRCS
    core/json_keyset_reader.cc
    json_keyset_reader.h
  DEPS
    tink::core::keyset_reader
    tink::util::enums
    tink::util::errors
    tink::util::protobuf_helper
    tink::util::statusor
    tink::proto::tink_cc_proto
    absl::memory
    absl::strings
    rapidjson
)

tink_cc_library(
  NAME json_keyset_writer
  SRCS
    core/json_keyset_writer.cc
    json_keyset_writer.h
  DEPS
    tink::core::keyset_writer
    tink::util::enums
    tink::util::errors
    tink::util::protobuf_helper
    tink::util::status
    tink::util::statusor
    tink::proto::tink_cc_proto
    absl::strings
    rapidjson
)

tink_cc_library(
  NAME catalogue
  SRCS catalogue.h
  DEPS
    tink::core::key_manager
    tink::util::statusor
    absl::base
)

tink_cc_library(
  NAME config
  SRCS
    core/config.cc
    config.h
  DEPS
    tink::core::catalogue
    tink::core::key_manager
    tink::core::registry
    tink::aead::aead_config
    tink::daead::deterministic_aead_config
    tink::hybrid::hybrid_config
    tink::mac::mac_config
    tink::signature::signature_config
    tink::streamingaead::streaming_aead_config
    tink::util::errors
    tink::util::status
    tink::util::statusor
    tink::proto::config_cc_proto
    absl::strings
)

tink_cc_library(
  NAME crypto_format
  SRCS
    core/crypto_format.cc
    crypto_format.h
  DEPS
    tink::util::errors
    tink::util::statusor
    tink::proto::tink_cc_proto
)

tink_cc_library(
  NAME primitive_set
  SRCS primitive_set.h
  DEPS
    tink::core::crypto_format
    tink::util::errors
    tink::util::statusor
    tink::proto::tink_cc_proto
    absl::memory
    absl::synchronization
)

tink_cc_library(
  NAME primitive_wrapper
  SRCS primitive_wrapper.h
  DEPS
    tink::core::primitive_set
    tink::util::statusor
)

tink_cc_library(
  NAME registry
  SRCS registry.h
  DEPS
    tink::core::registry_impl
    tink::util::status
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME registry_impl
  SRCS
    core/registry_impl.cc
    core/registry_impl.h
  DEPS
    tink::core::catalogue
    tink::core::key_manager
    tink::core::key_type_manager
    tink::core::key_manager_impl
    tink::core::private_key_manager_impl
    tink::core::private_key_type_manager
    tink::core::primitive_set
    tink::core::primitive_wrapper
    tink::util::errors
    tink::util::protobuf_helper
    tink::util::status
    tink::util::statusor
    tink::util::validation
    tink::proto::tink_cc_proto
    absl::base
    absl::strings
    absl::synchronization
)

configure_file(version.h.templ "${TINK_VERSION_H}")

tink_cc_library(
  NAME version
  SRCS
    core/version.cc
    "${TINK_VERSION_H}"
)

tink_cc_library(
  NAME keyset_handle
  SRCS
    core/keyset_handle.cc
    keyset_handle.h
  DEPS
    tink::core::aead
    tink::core::key_manager
    tink::core::keyset_reader
    tink::core::keyset_writer
    tink::core::primitive_set
    tink::core::registry
    tink::util::errors
    tink::proto::tink_cc_proto
    absl::memory
)

tink_cc_library(
  NAME cleartext_keyset_handle
  SRCS
    core/cleartext_keyset_handle.cc
    cleartext_keyset_handle.h
  DEPS
    tink::core::keyset_handle
    tink::util::errors
    tink::util::status
    tink::util::statusor
    tink::proto::tink_cc_proto
)

tink_cc_library(
  NAME key_manager
  SRCS
    core/key_manager.cc
    key_manager.h
  DEPS
    tink::util::errors
    tink::util::protobuf_helper
    tink::util::status
    tink::util::statusor
    tink::proto::tink_cc_proto
    absl::memory
    absl::strings
)

tink_cc_library(
  NAME keyset_manager
  SRCS
    core/keyset_manager.cc
    keyset_manager.h
  DEPS
    tink::core::keyset_handle
    tink::core::keyset_reader
    tink::core::registry
    tink::util::enums
    tink::util::errors
    tink::util::protobuf_helper
    tink::util::status
    tink::util::statusor
    tink::proto::tink_cc_proto
    absl::base
    absl::memory
    absl::synchronization
  PUBLIC
)

add_library(tink::keyset_manager ALIAS tink_core_keyset_manager)

tink_cc_library(
  NAME kms_client
  SRCS kms_client.h
  DEPS
    tink::core::aead
    tink::util::statusor
    absl::strings
)

tink_cc_library(
  NAME kms_clients
  SRCS
    core/kms_clients.cc
    kms_clients.h
  DEPS
    tink::core::kms_client
    tink::util::errors
    tink::util::status
    tink::util::statusor
    absl::base
    absl::strings
    absl::synchronization
)

tink_cc_library(
  NAME template_util
  SRCS
    core/template_util.h
  DEPS
    absl::meta
)

tink_cc_library(
  NAME key_type_manager
  SRCS
    core/key_type_manager.h
  DEPS
    absl::hash
    absl::flat_hash_map
    tink::core::input_stream
    tink::proto::tink_cc_proto
    tink::util::status
    tink::util::statusor
)

tink_cc_library(
  NAME key_manager_impl
  SRCS
    core/key_manager_impl.h
  DEPS
    tink::core::key_manager
    tink::core::key_type_manager
    tink::proto::tink_cc_proto
    tink::util::constants
    tink::util::status
    absl::base
)

tink_cc_library(
  NAME private_key_type_manager
  SRCS
    core/private_key_type_manager.h
  DEPS
    tink::core::key_type_manager
    tink::util::statusor
)

tink_cc_library(
  NAME private_key_manager_impl
  SRCS
    core/private_key_manager_impl.h
  DEPS
    tink::core::private_key_type_manager
    tink::core::key_manager_impl
    tink::core::key_manager
    tink::util::validation
)

if (TINK_BUILD_SHARED_LIB)
  add_library(tink SHARED
    ${TINK_PUBLIC_APIS}
    version_script.lds
    exported_symbols.lds
  )

  if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(tink
      PRIVATE
        -fuse-ld=gold  # GNU ld does not support ICF.
        -Wl,--version-script="${CMAKE_CURRENT_SOURCE_DIR}/version_script.lds"
        -Wl,--gc-sections
        -Wl,--icf=all
        -Wl,--strip-all
    )
  elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(tink
      PRIVATE
        -Wl,-install_name,@rpath/libtink.so
        -Wl,-exported_symbols_list="${CMAKE_CURRENT_SOURCE_DIR}/exported_symbols.lds"
        -Wl,-x
        -Wl,-dead_strip
    )
  else()
    message(FATAL_ERROR "libtink not yet supported on your platform")
  endif()

  target_include_directories(tink PUBLIC ${TINK_INCLUDE_DIRS})
  target_link_libraries(tink
    PRIVATE
      -Wl,--whole-archive
      ${TINK_PUBLIC_API_DEPS}
      -Wl,--no-whole-archive
  )
  set_target_properties(tink PROPERTIES SOVERSION ${TINK_VERSION_LABEL})

  install(TARGETS tink EXPORT Tink LIBRARY DESTINATION "lib")
endif()

# tests

tink_cc_test(
  NAME registry_test
  SRCS core/registry_test.cc
  DEPS
    tink::core::aead
    tink::core::catalogue
    tink::core::crypto_format
    tink::core::key_manager_impl
    tink::core::key_type_manager
    tink::core::keyset_manager
    tink::core::registry
    tink::aead::aead_wrapper
    tink::aead::aes_gcm_key_manager
    tink::hybrid::ecies_aead_hkdf_private_key_manager
    tink::hybrid::ecies_aead_hkdf_public_key_manager
    tink::util::test_keyset_handle
    tink::util::protobuf_helper
    tink::util::status
    tink::util::statusor
    tink::util::test_util
    tink::util::test_matchers
    tink::proto::aes_ctr_hmac_aead_cc_proto
    tink::proto::aes_gcm_cc_proto
    tink::proto::common_cc_proto
    tink::proto::ecdsa_cc_proto
    tink::proto::tink_cc_proto
    absl::memory
    gmock
)

tink_cc_test(
  NAME version_test
  SRCS core/version_test.cc
  DEPS
    tink::core::version
    gmock
)

tink_cc_test(
  NAME binary_keyset_reader_test
  SRCS core/binary_keyset_reader_test.cc
  DEPS
    tink::core::binary_keyset_reader
    tink::util::test_util
    tink::proto::tink_cc_proto
)

tink_cc_test(
  NAME binary_keyset_writer_test
  SRCS core/binary_keyset_writer_test.cc
  DEPS
    tink::core::binary_keyset_writer
    tink::util::test_util
    tink::proto::tink_cc_proto
)

tink_cc_test(
  NAME json_keyset_reader_test
  SRCS core/json_keyset_reader_test.cc
  DEPS
    tink::core::json_keyset_reader
    tink::util::protobuf_helper
    tink::util::test_util
    tink::proto::aes_eax_cc_proto
    tink::proto::aes_gcm_cc_proto
    tink::proto::tink_cc_proto
    absl::strings
)

tink_cc_test(
  NAME json_keyset_writer_test
  SRCS core/json_keyset_writer_test.cc
  DEPS
    tink::core::json_keyset_reader
    tink::core::json_keyset_writer
    tink::util::protobuf_helper
    tink::util::test_util
    tink::proto::aes_eax_cc_proto
    tink::proto::aes_gcm_cc_proto
    tink::proto::tink_cc_proto
    absl::strings
    rapidjson
)

tink_cc_test(
  NAME config_test
  SRCS core/config_test.cc
  DEPS
    tink::core::config
    tink::core::mac
    tink::proto::config_cc_proto
)

tink_cc_test(
  NAME crypto_format_test
  SRCS core/crypto_format_test.cc
  DEPS
    tink::core::crypto_format
    tink::proto::tink_cc_proto
)

tink_cc_test(
  NAME keyset_handle_test
  SRCS core/keyset_handle_test.cc
  DEPS
    tink::core::binary_keyset_reader
    tink::core::cleartext_keyset_handle
    tink::core::config
    tink::core::json_keyset_reader
    tink::core::json_keyset_writer
    tink::core::key_manager_impl
    tink::core::keyset_handle
    tink::static
    tink::aead::aead_key_templates
    tink::aead::aead_wrapper
    tink::aead::aes_gcm_key_manager
    tink::config::tink_config
    tink::signature::ecdsa_sign_key_manager
    tink::signature::signature_key_templates
    tink::util::test_keyset_handle
    tink::util::protobuf_helper
    tink::util::test_matchers
    tink::util::test_util
    tink::proto::tink_cc_proto
)

tink_cc_test(
  NAME key_manager_test
  SRCS core/key_manager_test.cc
  DEPS
    tink::core::key_manager
    tink::util::status
    tink::util::test_matchers
    tink::proto::empty_cc_proto
    gmock
)

tink_cc_test(
  NAME keyset_manager_test
  SRCS core/keyset_manager_test.cc
  DEPS
    tink::core::config
    tink::core::keyset_handle
    tink::core::keyset_manager
    tink::aead::aead_config
    tink::aead::aes_gcm_key_manager
    tink::util::test_keyset_handle
    tink::util::test_util
    tink::proto::aes_gcm_cc_proto
    tink::proto::tink_cc_proto
)

tink_cc_test(
  NAME cleartext_keyset_handle_test
  SRCS core/cleartext_keyset_handle_test.cc
  DEPS
    tink::core::binary_keyset_reader
    tink::core::cleartext_keyset_handle
    tink::core::keyset_handle
    tink::core::keyset_reader
    tink::util::test_keyset_handle
    tink::util::test_util
    tink::proto::tink_cc_proto
)

tink_cc_test(
  NAME primitive_set_test
  SRCS core/primitive_set_test.cc
  DEPS
    tink::core::crypto_format
    tink::core::mac
    tink::core::primitive_set
    tink::util::protobuf_helper
    tink::util::test_matchers
    tink::util::test_util
    tink::proto::tink_cc_proto
)

tink_cc_test(
  NAME kms_clients_test
  SRCS core/kms_clients_test.cc
  DEPS
    absl::strings
    tink::core::aead
    tink::core::kms_client
    tink::core::kms_clients
    tink::util::protobuf_helper
    tink::util::status
    tink::util::statusor
    tink::util::test_matchers
    tink::util::test_util
)

tink_cc_test(
  NAME template_util_test
  SRCS core/template_util_test.cc
  DEPS
    tink::core::template_util
)

tink_cc_test(
  NAME key_type_manager_test
  SRCS core/key_type_manager_test.cc
  DEPS
    tink::core::aead
    tink::core::key_type_manager
    tink::proto::aes_gcm_cc_proto
    tink::proto::tink_cc_proto
    tink::subtle::aes_gcm_boringssl
    tink::subtle::random
    tink::util::status
    tink::util::statusor
    tink::util::test_matchers
    tink::util::validation
)

tink_cc_test(
  NAME key_manager_impl_test
  SRCS core/key_manager_impl_test.cc
  DEPS
    tink::core::aead
    tink::core::key_manager_impl
    tink::proto::aes_gcm_cc_proto
    tink::proto::tink_cc_proto
    tink::subtle::aes_gcm_boringssl
    tink::util::status
    tink::util::statusor
    tink::util::test_matchers
    tink::util::validation
)

tink_cc_test(
  NAME private_key_manager_impl_test
  SRCS core/private_key_manager_impl_test.cc
  DEPS
    tink::core::key_manager_impl
    tink::core::private_key_type_manager
    tink::proto::ecdsa_cc_proto
    tink::proto::tink_cc_proto
    tink::subtle::aes_gcm_boringssl
    tink::util::status
    tink::util::statusor
    tink::util::test_matchers
    tink::util::validation
)
