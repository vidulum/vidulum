package=rust
$(package)_version=1.28.0
$(package)_download_path=https://static.rust-lang.org/dist
$(package)_file_name_linux=rust-$($(package)_version)-x86_64-unknown-linux-gnu.tar.gz
$(package)_sha256_hash_linux=2a1390340db1d24a9498036884e6b2748e9b4b057fc5219694e298bdaa37b810
$(package)_file_name_darwin=rust-$($(package)_version)-x86_64-apple-darwin.tar.gz
<<<<<<< HEAD
$(package)_sha256_hash_darwin=5d7a70ed4701fe9410041c1eea025c95cad97e5b3d8acc46426f9ac4f9f02393
$(package)_file_name_mingw32=rust-$($(package)_version)-x86_64-pc-windows-gnu.tar.gz
$(package)_sha256_hash_mingw32=55c07426f791c51c8a2b6934b35784175c4abb4e03f123f3e847109c4dc1ad8b
=======
$(package)_sha256_hash_darwin=f0dfba507192f9b5c330b5984ba71d57d434475f3d62bd44a39201e36fa76304
>>>>>>> d7ab95451... depends: Generalise the rust package cross-compilation functions

# Mapping from GCC canonical hosts to Rust targets
# If a mapping is not present, we assume they are identical
$(package)_rust_target_x86_64-w64-mingw32=x86_64-pc-windows-gnu

# Mapping from Rust targets to SHA-256 hashes
<<<<<<< HEAD
$(package)_rust_std_sha256_hash_aarch64-unknown-linux-gnu=346efe3aef2aff7b71a611bf7661bcec5f9bc4025a599c2866ec5fd330247cb9
=======
>>>>>>> d7ab95451... depends: Generalise the rust package cross-compilation functions
$(package)_rust_std_sha256_hash_x86_64-pc-windows-gnu=cad5f1454d591c13eeb3657f1c9dbfeb30e648f59680bd0765b94c63e7afc49e

ifneq ($(canonical_host),$(build))
$(package)_rust_target=$(if $($(package)_rust_target_$(canonical_host)),$($(package)_rust_target_$(canonical_host)),$(canonical_host))
$(package)_exact_file_name=rust-std-$($(package)_version)-$($(package)_rust_target).tar.gz
$(package)_exact_sha256_hash=$($(package)_rust_std_sha256_hash_$($(package)_rust_target))
$(package)_build_subdir=buildos
$(package)_extra_sources=$($(package)_file_name_$(build_os))

define $(package)_fetch_cmds
$(call fetch_file,$(package),$($(package)_download_path),$($(package)_download_file),$($(package)_file_name),$($(package)_sha256_hash)) && \
$(call fetch_file,$(package),$($(package)_download_path),$($(package)_file_name_$(build_os)),$($(package)_file_name_$(build_os)),$($(package)_sha256_hash_$(build_os)))
endef

define $(package)_extract_cmds
  mkdir -p $($(package)_extract_dir) && \
  echo "$($(package)_sha256_hash)  $($(package)_source)" > $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  echo "$($(package)_sha256_hash_$(build_os))  $($(package)_source_dir)/$($(package)_file_name_$(build_os))" >> $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  $(build_SHA256SUM) -c $($(package)_extract_dir)/.$($(package)_file_name).hash && \
  mkdir $(canonical_host) && \
  tar --strip-components=1 -xf $($(package)_source) -C $(canonical_host) && \
  mkdir buildos && \
  tar --strip-components=1 -xf $($(package)_source_dir)/$($(package)_file_name_$(build_os)) -C buildos
endef

define $(package)_stage_cmds
  ./install.sh --destdir=$($(package)_staging_dir) --prefix=$(host_prefix)/native --disable-ldconfig && \
  ../$(canonical_host)/install.sh --destdir=$($(package)_staging_dir) --prefix=$(host_prefix)/native --disable-ldconfig
endef
else

define $(package)_stage_cmds
  ./install.sh --destdir=$($(package)_staging_dir) --prefix=$(host_prefix)/native --disable-ldconfig
endef
endif
