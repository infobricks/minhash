require 'test/unit'
require 'minhash'

class TestMinHash < Test::Unit::TestCase

  def test_create
    g = MinHash::Generator.new()
    assert_not_nil(g)
  end

  def test_create_minhash
    g = MinHash::Generator.new()
    mh = g.to_minhash(%w(I eat an apple))
    assert_not_nil(mh)
  end

  def test_same
    g = MinHash::Generator.new()
    x = g.to_minhash(%w(I eat an apple))
    y = g.to_minhash(%w(I eat an apple))
    assert_equal(1.0, x.jaccard(y))
  end

end
