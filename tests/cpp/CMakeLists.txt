add_executable(test_writer test_writer.cc)
target_link_libraries(test_writer PRIVATE hpq_core)
add_test(NAME test_writer COMMAND test_writer)

add_executable(test_encodings test_encodings.cc)
target_link_libraries(test_encodings PRIVATE hpq_core)
add_test(NAME test_encodings COMMAND test_encodings)

add_executable(test_gpu_compress test_gpu_compress.cc)
target_link_libraries(test_gpu_compress PRIVATE hpq_core)
add_test(NAME test_gpu_compress COMMAND test_gpu_compress)

add_executable(test_adaptive test_adaptive.cc)
target_link_libraries(test_adaptive PRIVATE hpq_core)
add_test(NAME test_adaptive COMMAND test_adaptive)

add_executable(test_dict test_dict.cc)
target_link_libraries(test_dict PRIVATE hpq_core)
add_test(NAME test_dict COMMAND test_dict)

add_executable(test_delta test_delta.cc)
target_link_libraries(test_delta PRIVATE hpq_core)
add_test(NAME test_delta COMMAND test_delta)

add_executable(test_bloom test_bloom.cc)
target_link_libraries(test_bloom PRIVATE hpq_core)
add_test(NAME test_bloom COMMAND test_bloom)

