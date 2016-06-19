Gem::Specification.new do |spec|
  spec.extensions << 'ext/Rakefile'
  spec.name = "minhash"
  spec.version = "0.0.1"
  spec.authors = ["Hayato Itsumi"]
  spec.email = "hayato.itsumi@icloud.com"
  spec.summary = %q{b-bit MinHash implementation for Ruby with FFI C extension}
  spec.license = %q{Confidential}

  spec.files = %w(minhash.gemspec) + Dir.glob("{lib,spec,ext}/**/*")
  spec.test_files = Dir.glob("{test,spec,features}")

  spec.add_dependency "rake", "~> 10.4"
  spec.add_dependency "ffi-compiler", "~> 0.1"
end
