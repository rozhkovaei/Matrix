#include <iostream>
#include <map>
#include <tuple>
#include <iterator>

template< typename T, T default_value, size_t dimensions = 2 >
class matrix
{
public:
    
    matrix() : iterator_( elements_.begin(), &elements_) {}
    
    auto& operator[]( size_t idx1 )
    {
        return elements_[ idx1 ];
    }

    auto size() const
    {
        size_t count = 0;
        for( const auto& element : elements_ )
            count += element.second.size();
        return count;
    }
    
    void start()
    {
        iterator_.set( elements_.begin() );
    }
    
    class iterator
    {
        typename std::map< size_t, matrix< T, default_value, dimensions-1 > >::iterator current_;
        std::map< size_t, matrix< T, default_value, dimensions-1 > >* elements_;
        
    public:
        
        iterator( typename std::map< size_t, matrix< T, default_value, dimensions-1 > >::iterator iter,
                 std::map< size_t, matrix< T, default_value, dimensions-1 > >* elements = nullptr )
        : current_( iter )
        , elements_(elements)
        {
        }
        
        iterator( const iterator& other ) : current_( other.current_ ), elements_( other.elements_ ) {}
        
        iterator &operator=( const iterator &other )
        {
            this->current_ = other.current_;
            this->elements_ = other.elements_;
            return *this;
        }
        
        void set( typename std::map< size_t, matrix< T, default_value, dimensions-1 > >::iterator iter )
        {
            current_ = iter;
            if( elements_ && current_ != elements_->end() )
                current_->second.start();
        }
        
        iterator& next()
        {
            if( current_->second.iter().next() == current_->second.end() )
            {
                current_++;
                if( elements_ && current_ != elements_->end() )
                    current_->second.start();
            }
            return *this;
        }
        
        iterator& operator++()
        {
            return next();
        }
        
        bool operator==( iterator other ) const { return current_ == other.current_; }
        bool operator!=( iterator other ) const { return !( *this == other ); }
        
        auto operator*()
        {
            return std::tuple_cat( std::tuple< int >( current_->first ), *current_->second.iter() );
        }
    
        using iterator_category = std::forward_iterator_tag;
    };

    iterator& iter() { return iterator_; }
    
    iterator begin()
    {
        clear(); // скорее всего, костыль, но лучше не придумала - очистка пустых мап (появляются пустые, если сначала добавили значение, а потом присвоили ему дефолтное)
        start();
        return { elements_.begin(), &elements_ };
    }
    iterator end() { return { elements_.end(), &elements_ }; }
    
private:
    
    void clear()
    {
        for ( auto i = elements_.begin(), last = elements_.end(); i != last; )
        {
            if ( i->second.size() == 0 )
                i = elements_.erase( i );
            else
                ++i;
        }
    }
    
    std::map< size_t, matrix< T, default_value, dimensions-1 > > elements_;
    iterator iterator_;
};

// этот класс использую для того, чтобы не добавлять в матрицу дефолтные значения
template < typename T, T default_value >
class handler
{
public:
    
    handler( size_t idx, std::map<size_t, T>& elements ) : idx_( idx ), elements_( elements ) {}
    
    T operator*()
    {
        return value();
    }
    
    T value() const
    {
        if( elements_.find( idx_ ) != elements_.end() )
            return elements_[ idx_ ];
        return default_value;
    }
    
    auto operator=( const T &other_value )
    {
        if( other_value != default_value )
        {
            elements_[ idx_ ] = other_value;
        }
        else if( elements_.find( idx_ ) != elements_.end() )
        {
            elements_.erase( idx_ );
        }
        
        return this;
    }
    
    bool operator==( const T &other_value )
    {
        if( elements_.find( idx_ ) == elements_.end() )
        {
            return other_value == default_value;
        }
        return elements_[ idx_ ] == other_value;
    }
    
private:
    size_t idx_;
    std::map< size_t, T >& elements_;
};

template < typename T, T default_value >
class matrix< T, default_value, 1 >
{
public:

  //  matrix() = default;
    
    auto operator[]( size_t idx )
    {
        return handler<T, default_value>{ idx, elements_ };
    }

    auto size() const
    {
        return std::count_if( elements_.begin(), elements_.end(),
                             []( auto element ) { return element.second != default_value; } );
    }
    
    void start()
    {
        iterator_.set( elements_.begin() );
    }
    
    class iterator
    {
        typename std::map<size_t, T>::iterator current_;
        
    public:
        
        iterator( typename std::map<size_t, T>::iterator iter )
        : current_( iter )
        {}
        
        iterator(const iterator& other) { current_ = other.current_; }
        
        void set( typename std::map<size_t, T>::iterator iter )
        {
            current_ = iter;
        }
        
        iterator& next()
        {
            ++current_;
            return *this;
        }
        
        iterator &operator++()
        {
            return next();
        }
        
        iterator &operator=( const iterator &other )
        {
            this->current_ = other.current_;
            return *this;
        }
        
        bool operator==( iterator other ) const { return current_ == other.current_; }
        bool operator!=( iterator other ) const { return !( *this == other ); }

        std::tuple< int, T > operator*()
        {
            return std::tuple< int, T >{ current_->first, current_->second };
        }
        
        using iterator_category = std::forward_iterator_tag;
    };

    iterator& iter() { return iterator_; }
    iterator begin() { return { elements_.begin() }; }
    iterator end() { return { elements_.end() }; }
    
private:
    std::map< size_t, T > elements_;
    iterator iterator_ = elements_.begin();
};

template< typename T, T default_value >
std::ostream &operator<<( std::ostream &output, const handler<T, default_value>& other )
{
    output << other.value();
    return output;
}

int main()
{
    /* matrix<int, -1> _matrix;
    assert(_matrix.size() == 0); // все ячейки свободны
    
    auto a = _matrix[1][0];
    assert(a == -1);
    assert(_matrix.size() == 0);
    
    _matrix[100][100] = 314;
    assert(_matrix[100][100] == 314);
    assert(_matrix.size() == 1);
        
    // 100100314
    for(auto c: _matrix)
    {
        int x;
        int y;
        int v;
        std::tie(x, y, v) = c;
        std::cout << x << y << v << std::endl;
    }*/
    
    matrix<int, 0> matrix_fill;
    
    for(int i = 0; i < 10; ++i)
    {
        matrix_fill[i][i] = i;
        matrix_fill[9-i][i] = 9-i;
    }
    
    for(int x = 1; x < 9; ++x)
    {
        for(int y = 1; y < 9; ++y)
        {
            std::cout << matrix_fill[x][y];
            if(y != 8)
                std::cout << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    
    std::cout << matrix_fill.size() << std::endl;
    
    std::cout << std::endl;
    
    for(auto c: matrix_fill)
    {
        int x;
        int y;
        int v;
        std::tie(x, y, v) = c;
        std::cout << x << y << v << std::endl;
    }
    
   // тест для 3-х мерной матрицы (рабочий)
    /*
    matrix<int, -1, 3> _matrix2;
    _matrix2[0][0][1] = 100;
    _matrix2[0][0][2] = 100;
    _matrix2[0][1][20] = 300;
    _matrix2[0][1][30] = 30;
    _matrix2[20][41][90] = 800;
    
    _matrix2[0][1][20] = -1;
    _matrix2[0][0][1] = -1;
 //   _matrix2[20][41][90] = -1;
    
    std::cout << _matrix2.size() << std::endl;
    
    for(auto c: _matrix2)
    {
        int x;
        int y;
        int z;
        int v;
        std::tie(x, y, z, v) = c;
        std::cout << x << y << z << v << std::endl;
    }*/
    
    return 0;
}
