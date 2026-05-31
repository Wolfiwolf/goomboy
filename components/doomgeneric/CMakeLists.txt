
idf_component_register(SRC_DIRS "."
		       INCLUDE_DIRS "." "${CMAKE_CURRENT_BINARY_DIR}")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=char-subscripts -Wno-error=unused-value -Wno-error=parentheses -Wno-error=int-to-pointer-cast -Wno-pointer-sign -Wno-error=unused-but-set-parameter -Wno-error=maybe-uninitialized -Wno-error=unused-const-variable -Wno-error=misleading-indentation -Wno-error=nonnull -Wno-error=dangling-else -Wno-error=format-overflow -Wno-error=duplicate-decl-specifier -Wno-error=sizeof-pointer-div -Wno-error=char-subscripts -Wno-error=unused-value -Wno-error=parentheses -Wno-error=int-to-pointer-cast -Wno-pointer-sign -Wno-unused-but-set-variable -Wno-maybe-uninitialized)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-dangling-pointer)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-but-set-parameter)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-variable)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-misleading-indentation)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)



# PrBoom is C89-era code: pin the dialect to gnu17 (GCC 15 defaults to C23, which
# makes true/false/bool keywords and empty () mean (void)), and use -fpermissive to
# downgrade GCC 14's incompatible-pointer-types errors back to warnings.
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu17 -fpermissive)
