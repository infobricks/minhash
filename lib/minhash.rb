require 'ffi'
require 'ffi-compiler/loader'

module MinHash
  extend FFI::Library
  ffi_lib FFI::Compiler::Loader.find("minhash")

  class BitVectorStruct < FFI::Struct
    layout(
      :bits, :long,
      :num_container, :uint,
      :container, :pointer,
    )
  end

  class Value
    attr_accessor :bitvector

    def initialize(bitvector, b)
      @bitvector = bitvector
      @b = b
      @k = @bitvector[:bits]/@b
    end

    def jaccard(other)
      unless other.class == self.class
        raise TypeError, "jaccard calc require BitVector class"
      end
      MinHash.jaccard_c(@bitvector, other.bitvector, @k, @b)
    end

  end

  class Generator
    def initialize(k=128, b=1)
      @k = k
      @b = b
      @s = []
      @k.times { @s << rand(1000000) }
      @seeds = FFI::MemoryPointer.new(:uint32, @k)
      @seeds.write_array_of_int(@s)
    end

    def to_minhash(words)
      ptrs = FFI::MemoryPointer.new(:pointer, words.size)
      arry = []
      words.each do |w|
        str = FFI::MemoryPointer.new(:string, w.size)
        str.write_string(w)
        arry << str
      end
      ptrs.write_array_of_pointer(arry)
      bv = MinHash.to_minhash_c(ptrs, words.size, @seeds, @k, @b)
      MinHash::Value.new(bv,@b)
    end
  end

  protected

  attach_function :to_minhash_c, [:pointer, :uint32, :pointer, :uint32, :uint32], BitVectorStruct.ptr
  attach_function :jaccard_c, [BitVectorStruct.ptr, BitVectorStruct.ptr, :uint32, :uint32], :float
 
end
