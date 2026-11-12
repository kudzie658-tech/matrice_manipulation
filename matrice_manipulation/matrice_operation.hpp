#pragma once

#include "matrice.hpp"
#include<memory>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <iomanip>
#include <utility>
#include <optional>

using MatrixVector = std::vector<long double>;
class MatriceOperation
{
};

template<typename ConstraintType, typename ValueType>
struct Result
{
    ConstraintType error_code;
    ValueType value;
};

class UnaryOperation : public MatriceOperation
{
	protected: 
		std::shared_ptr<Matrice> m_matrice;
    public:
	    virtual bool execute(){}
};

class BinaryOperation : public MatriceOperation
{
	protected:std::shared_ptr<Matrice> m_left_hand_side, m_right_hand_side;

public:
	BinaryOperation(Matrice& lhs, Matrice& rhs): m_left_hand_side{std::make_shared<Matrice>(lhs)}, m_right_hand_side{std::make_shared<Matrice>(rhs)}{}
    virtual Matrice execute() { return Matrice{}; }
};

class Determinant:public UnaryOperation
{
    private:
	    double m_answer;

      enum class DeterminantMatriceConstraints
        {
            RECTANGULAR,
            INFINITE_VALUE,
            NUMERIC_OVERFLOW,
            SINGULAR,
            DETERMINANT_OVERFLOW,
            OK
        };

      using DeterminantTypeResult  = Result< DeterminantMatriceConstraints, long double>;
	    DeterminantTypeResult gussian_solver()
	    {
            auto matrix = m_matrice->get_matrice();
            int matrix_size = m_matrice->get_order().first * m_matrice->get_order().second;

            if (matrix_size == 0)
            {
                return DeterminantTypeResult{
                    DeterminantMatriceConstraints::OK,
                    0.0L
                };
            }

            if (matrix_size > matrix.max_size() / matrix_size || matrix.size() != matrix_size * matrix_size)
            {
                return DeterminantTypeResult{
                   DeterminantMatriceConstraints::RECTANGULAR,
                   0.0L
                };
            }

            for (const auto value : matrix)
            {
                if (!std::isfinite(value))
                {
                    return DeterminantTypeResult{
                   DeterminantMatriceConstraints::INFINITE_VALUE,
                   0.0L
                    };
                }
            }

            int sign = 1;

            // Gaussian elimination with partial pivoting.
            for (std::size_t col = 0; col < matrix_size; ++col)
            {
                std::size_t pivotRow = col;

                // Find the largest absolute pivot.
                for (std::size_t row = col + 1; row < matrix_size; ++row)
                {
                    if (std::abs(matrix[row * matrix_size + col]) >
                        std::abs(matrix[pivotRow * matrix_size + col]))
                    {
                        pivotRow = row;
                    }
                }

                const long double pivot =
                    matrix[pivotRow * matrix_size + col];

                if (pivot == 0.0L)
                {
                    return DeterminantTypeResult{
                   DeterminantMatriceConstraints::SINGULAR,
                   0.0L
                    };
                }

                // Swap complete rows.
                    if (pivotRow != col)
                    {
                        for (std::size_t j = 0; j < matrix_size; ++j)
                        {
                            std::swap(
                                matrix[col * matrix_size + j],
                                matrix[pivotRow * matrix_size + j]
                            );
                        }

                        sign *= -1;
                    }

                const long double diagonal = matrix[col * matrix_size + col];

                // Eliminate elements below the pivot.
                for (std::size_t row = col + 1; row < matrix_size; ++row)
                {
                    const long double factor =
                        matrix[row * matrix_size + col] / diagonal;

                    if (!std::isfinite(factor))
                    {
                        return DeterminantTypeResult{
                                DeterminantMatriceConstraints::NUMERIC_OVERFLOW,
                                0.0L
                        };
                    }

                    matrix[row * matrix_size + col] = 0.0L;

                    for (std::size_t j = col + 1; j < matrix_size; ++j)
                    {
                        matrix[row * matrix_size + j] -=
                            factor * matrix[col * matrix_size + j];

                        if (!std::isfinite(matrix[row * matrix_size + j]))
                        {
                            return DeterminantTypeResult{
                                DeterminantMatriceConstraints::NUMERIC_OVERFLOW,
                                0.0L };
                        }
                    }
                }
            }

            // Calculate the determinant.
            long double result = static_cast<long double>(sign);

            for (std::size_t i = 0; i < matrix_size; ++i)
            {
                result *= matrix[i * matrix_size + i];

                if (!std::isfinite(result))
                {
                    return DeterminantTypeResult{
                                DeterminantMatriceConstraints::DETERMINANT_OVERFLOW,
                                0.0L 
                    };
                }
            }

            return DeterminantTypeResult{
                                DeterminantMatriceConstraints::OK,
                                result
            };
        }

	public:
        Determinant(Matrice& matrice) : m_answer{ 0 }
        {
            m_matrice = std::make_shared<Matrice>(matrice);
        }
        double get_result() { return m_answer; }
		bool execute() override
		{
            auto result = gussian_solver();
            bool success = result.error_code == DeterminantMatriceConstraints::OK || result.error_code == DeterminantMatriceConstraints::SINGULAR;

            m_answer =result.value;

            return success;
		}
};

class Inverse : public UnaryOperation
{
	std::shared_ptr< Matrice > m_inverse_matrice;

     enum class InverseMatriceConstraints
    {
         EMPTY,
         RECTANGULAR,
         INFINITE_VALUE,
         NUMERIC_OVERFLOW,
         SINGULAR,
         OK
    };

     using InverseTypeResult = Result< InverseMatriceConstraints, Matrice>;
     InverseTypeResult inverse_matrix(MatrixVector matrix, std::size_t n )
    {
        if (n == 0) 
        {
            return InverseTypeResult{
                    InverseMatriceConstraints::EMPTY,
                    Matrice({})};
        }

        if (n > matrix.max_size() / n || matrix.size() != n * n)
        {
            return InverseTypeResult{
                    InverseMatriceConstraints::RECTANGULAR,
                    Matrice({}) };
        }

        for (const auto value : matrix)
        {
            if (!std::isfinite(value))
            {
                return InverseTypeResult{
                        InverseMatriceConstraints::INFINITE_VALUE,
                        Matrice({}) };
            }
        }

        // Create an identity matrix.
        MatrixVector identity_vec(n * n, 0.0L);

        for (std::size_t i = 0; i < n; ++i)
        {
            identity_vec[i * n + i] = 1.0L;
        }

        // Gauss-Jordan elimination with partial pivoting.
        for (std::size_t col = 0; col < n; ++col)
        {
            // Find the best pivot.
            std::size_t pivotRow = col;

            for (std::size_t row = col + 1; row < n; ++row)
            {
                if (std::abs(matrix[row * n + col]) >
                    std::abs(matrix[pivotRow * n + col]))
                {
                    pivotRow = row;
                }
            }

            const long double pivot{ matrix[pivotRow * n + col] };

            // Singular matrix.
            if (pivot == 0.0L)
            {
                return InverseTypeResult{ InverseMatriceConstraints::SINGULAR, Matrice({})};
            }

            // Swap corresponding rows in both matrices.
            if (pivotRow != col)
            {
                for (std::size_t j = 0; j < n; ++j)
                {
                    std::swap(
                        matrix[col * n + j],
                        matrix[pivotRow * n + j]
                    );

                    std::swap(
                        identity_vec[col * n + j],
                        identity_vec[pivotRow * n + j]
                    );
                }
            }

            const long double diagonal = matrix[col * n + col];

            // Normalize the pivot row.
            for (std::size_t j = 0; j < n; ++j)
            {
                matrix[col * n + j] /= diagonal;
                identity_vec[col * n + j] /= diagonal;

                if (!std::isfinite(matrix[col * n + j]) ||
                    !std::isfinite(identity_vec[col * n + j]))
                {
                    return InverseTypeResult{
                            InverseMatriceConstraints::NUMERIC_OVERFLOW,
                            Matrice({}) };
                }
            }

            // Eliminate the pivot column in every other row.
            for (std::size_t row = 0; row < n; ++row)
            {
                if (row == col)
                {
                    continue;
                }

                const long double factor{ matrix[row * n + col] };

                for (std::size_t j = 0; j < n; ++j)
                {
                    if (j != col)
                    {
                        matrix[row * n + j] -=
                            factor * matrix[col * n + j];
                    }

                    identity_vec[row * n + j] -=
                        factor * identity_vec[col * n + j];

                    if (!std::isfinite(matrix[row * n + j]) ||
                        !std::isfinite(identity_vec[row * n + j]))
                    {
                        return InverseTypeResult{
                                InverseMatriceConstraints::NUMERIC_OVERFLOW,
                                Matrice({}) };
                    }
                }

                matrix[row * n + col] = 0.0L;
            }
        }

        auto order = m_matrice->get_order();
        Matrice identity{identity_vec,order};
        return InverseTypeResult{ InverseMatriceConstraints::OK, identity};
    }

public:
	Inverse(Matrice& matrice)
	{
		m_matrice = std::make_shared<Matrice>(matrice);
	}

    std::shared_ptr< Matrice > get_result() { return m_inverse_matrice; }
	
	bool execute() override
	{
        int matrix_size = m_matrice->get_order().first * m_matrice->get_order().second;
        auto result = inverse_matrix(m_matrice->get_matrice(), matrix_size);
        bool success = result.error_code == InverseMatriceConstraints::OK;
        m_inverse_matrice = std::make_shared< Matrice >(result.value);
        return success;
	}
};

class Adder :public BinaryOperation
{
public:
	Adder(Matrice& lhs, Matrice& rhs): BinaryOperation(rhs,lhs){}
	Matrice execute() override
	{
		return *m_left_hand_side + *m_right_hand_side;
	}
};

class Subtracter : public BinaryOperation
{
public:
	Subtracter(Matrice& lhs, Matrice& rhs): BinaryOperation(lhs,rhs){}
	Matrice execute() override
	{
		return *m_left_hand_side - *m_right_hand_side;
	}
};

class Multiplier : public BinaryOperation
{
public:
	Multiplier(Matrice& lhs, Matrice& rhs): BinaryOperation(lhs,rhs){}

	Matrice execute() override
	{
		return (*m_left_hand_side) * (*m_right_hand_side);
	}
};