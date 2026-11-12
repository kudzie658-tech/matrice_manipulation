#include "matrice.hpp"
#include <iomanip>
#include <iostream>

using MatrixVector = std::vector<long double>;

Matrice::Matrice(std::initializer_list<long double> matrice) : m_order{ 3, 3 }
{
    for (auto e : matrice)
    {
        m_matrice.push_back(e);
    }
}
Matrice::Matrice(MatrixVector&& vec, std::pair<int, int>&& order)
    : m_matrice{ vec }, m_order{ order }
{
}
Matrice::Matrice(MatrixVector& vec, std::pair<int, int>& order) : Matrice(std::move(vec), std::move(order))
{
}
Matrice::Matrice(Matrice&& M): Matrice(M.get_matrice(), M.get_order()) {}
Matrice::Matrice(Matrice& M) : Matrice(std::move(M)) {}
Matrice Matrice::get_negative_identity()
{
    MatrixVector e;

    for (int row{ 0 }; row < m_order.first; row++)
    {
        e.push_back(-1);
        for (int col{ 0 }; col < m_order.second; col++)
            e.push_back(0);
    }

    return Matrice { MatrixVector(std::move(e)), std::move(m_order)};
}
Matrice& Matrice::operator=(Matrice&& other)
{
    auto mat{ other.get_matrice() };
    m_order = other.get_order();
    for (auto e : mat)
    {
        m_matrice.push_back(e);
    }
    return *this;
}
Matrice& Matrice::operator=(Matrice& other)
{
    return this->operator=(std::move(other));
}

void Matrice::print()
{
    int matrice_size = m_order.first * m_order.second;

        for (std::size_t i = 0; i < matrice_size; ++i)
        {
                std::cout << std::setw(7) << m_matrice[i];

                if ((i + 1) % m_order.second == 0)
                {
                    std::cout << '\n';
                }
        }
}

Matrice Matrice::operator*(Matrice&& other)
{
    Matrice result{};
    MatrixVector resultant_vector;

    auto vec1 = this->get_matrice();
    auto vec2 = other.get_matrice();
    int rows = m_order.first;
    int cols = m_order.second;

    auto get_pos = [&](int order, int row, int idx) { return order * row + idx; };

    auto compute_rows = [&](int pass)
        {
            for (int row{ 0 }; row < rows; row++)
            {
                double accumulator = 0;
                for (int col{ 0 }; col < cols; col++)
                {
                    accumulator += vec1.at(get_pos(rows, pass, col)) *
                        vec2.at(get_pos(other.get_order().second,col, row));
                }
                resultant_vector.push_back(accumulator);
            }
        };

    for (int i{ 0 }; i < cols; ++i)
    {
        compute_rows(i);
    }

    result.set_matrice(std::move(resultant_vector));
    result.set_order(std::move(other.get_order()));

    return result;
}
Matrice Matrice::operator*(Matrice& other)
{
    return this->operator*(std::move(other));
}
Matrice Matrice::operator+(Matrice&& other)
{
    Matrice result{};
    MatrixVector resultant_vector;
    auto vec1 = this->get_matrice();
    auto vec2 = other.get_matrice();
    int rows = m_order.second;
    int cols = rows;
    auto get_pos = [&](int row, int idx) { return rows * row + idx; };

    for (int row{ 0 }; row < rows; row++)
    {
        for (int col{ 0 }; col < cols; col++)
        {
            resultant_vector.push_back(vec1.at(get_pos(row, col)) +
                vec2.at(get_pos(row, col)));
        }
    }

    result.set_matrice(std::move(resultant_vector));
    result.set_order(std::move(m_order));

    return result;
}
Matrice Matrice::operator+(Matrice& other)
{
    return this->operator+(std::move(other));
}
Matrice Matrice::operator-(Matrice&& other)
{
    Matrice result{};
    Matrice negative_identity{ get_negative_identity() };

    result = this->operator+(negative_identity * other);
    return result;
}
Matrice Matrice::operator-(Matrice& other)
{
    return this->operator-(std::move(other));
}
