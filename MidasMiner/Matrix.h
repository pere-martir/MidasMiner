#ifndef MidasMiner_Matrix_h
#define MidasMiner_Matrix_h

#include <vector>
#include <sstream>

class Matrix
{
private:
    std::vector<unsigned> m_data; // Row-major order
    unsigned m_columns, m_rows;
    
public:
    
    Matrix(unsigned rows = 0, unsigned columns = 0) : m_columns(columns), m_rows(rows)
    {
        m_data.resize(rows * columns, 0);
    }
    
    void initWithElements(unsigned rows, unsigned columns, ...) {
        va_list vl;
        va_start(vl, columns);
        m_columns = columns;
        m_rows = rows;
        unsigned elements = m_columns * m_rows;
        m_data.clear();
        for (unsigned i = 0; i < elements; ++ i)
            m_data.push_back(va_arg(vl, unsigned));
        
        va_end(vl);
    }
    
    bool inside(unsigned y, unsigned x) const {
        return y < m_rows & x < m_columns;
    }
    
    unsigned operator () (unsigned y, unsigned x) const {
        assert(inside(y, x));
        unsigned index = y * m_columns + x;
        return m_data[index];
    }
    
    unsigned& operator () (unsigned y, unsigned x) {
        assert(inside(y, x));
        unsigned index = y * m_columns + x;
        return m_data[index];
    }
    
    bool operator == (const Matrix& other) const { return m_data == other.m_data; }
    
    std::string string() const
    {
        std::stringstream ss;
        for (unsigned int j = 0; j < rows(); ++ j) {
            for (unsigned int i = 0; i < columns(); ++ i) {
                ss << operator() (j, i) << " ";
            }
            ss << std::endl;
        }
        return ss.str();
    }
    
    unsigned columns() const { return m_columns; }
    unsigned rows() const { return m_rows; }
    unsigned elements() const { return m_columns * m_rows; }
    unsigned getElementByIndex(unsigned index) const {
        assert(index < m_data.size());
        return m_data[index];
    }
};


#endif
