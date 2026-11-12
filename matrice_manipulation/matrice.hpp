#pragma once

#include <algorithm>
#include <ranges>
#include <vector>
#include<print>


using MatrixVector = std::vector<long double>;
class Matrice
{
   

private:
    MatrixVector m_matrice{};
    std::pair<int, int> m_order{ 0, 0 };

public:
    MatrixVector& get_matrice() { return m_matrice; }
    std::pair<int, int>& get_order() { return m_order; }

    void set_order(std::pair<int, int>&& order) { m_order = order; }
    void set_matrice(MatrixVector&& matrice) { m_matrice = matrice; }

    Matrice(std::initializer_list<long double> matrice);

    Matrice(MatrixVector&& vec, std::pair<int, int>&& order);
    Matrice(MatrixVector& vec, std::pair<int, int>& order);

    Matrice(Matrice&& M);
    Matrice(Matrice& M);

    Matrice get_negative_identity();

    Matrice& operator=(Matrice&& other);

    Matrice& operator=(Matrice& other);

    void print();

    Matrice operator*(Matrice&& other);
    Matrice operator*(Matrice& other);
    Matrice operator+(Matrice&& other);
    Matrice operator+(Matrice& other);
    Matrice operator-(Matrice&& other);
    Matrice operator-(Matrice& other);
};