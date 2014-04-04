/*
 *  Copyright 2008-2009 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*! \file csrb_matrix.h
 *  \brief Compressed Sparse Row matrix format.
 */

#pragma once

#include <cusp/detail/config.h>

#include <cusp/array1d.h>
#include <cusp/format.h>
#include <cusp/detail/matrix_base.h>

namespace cusp
{

// forward definition
template <typename Array1, typename Array2, typename IndexType, typename MemorySpace> class csrb_matrix_view;

/*! \addtogroup sparse_matrices Sparse Matrices
 */

/*! \addtogroup sparse_matrix_containers Sparse Matrix Containers
 *  \ingroup sparse_matrices
 *  \{
 */

/*! \p csrb_matrix : Compressed Sparse Row matrix container
 *
 * \tparam IndexType Type used for matrix indices (e.g. \c int).
 * \tparam MemorySpace A memory space (e.g. \c cusp::host_memory or cusp::device_memory)
 *
 * \note The matrix entries within the same row must be sorted by column index.
 * \note The matrix should not contain duplicate entries.
 *
 *  The following code snippet demonstrates how to create a 4-by-3
 *  \p csrb_matrix on the host with 6 nonzeros and then copies the
 *  matrix to the device.
 *
 *  \code
 *  #include <cusp/csrb_matrix.h>
 *  ...
 *
 *  // allocate storage for (4,3) matrix with 4 nonzeros
 *  cusp::csrb_matrix<int,float,cusp::host_memory> A(4,3,6);
 *
 *  // initialize matrix entries on host
 *  A.row_offsets[0] = 0;  // first offset is always zero
 *  A.row_offsets[1] = 2;  
 *  A.row_offsets[2] = 2;  
 *  A.row_offsets[3] = 3;
 *  A.row_offsets[4] = 6; // last offset is always num_entries
 *
 *  A.column_indices[0] = 0; A.values[0] = 10; 
 *  A.column_indices[1] = 2; A.values[1] = 20;
 *  A.column_indices[2] = 2; A.values[2] = 30;
 *  A.column_indices[3] = 0; A.values[3] = 40;
 *  A.column_indices[4] = 1; A.values[4] = 50;
 *  A.column_indices[5] = 2; A.values[5] = 60;
 *
 *  // A now represents the following matrix
 *  //    [10  0 20]
 *  //    [ 0  0  0]
 *  //    [ 0  0 30]
 *  //    [40 50 60]
 *
 *  // copy to the device
 *  cusp::csrb_matrix<int,float,cusp::device_memory> A = B;
 *  \endcode
 *
 */
template <typename IndexType, class MemorySpace>
class csrb_matrix : public detail::binary_matrix_base<IndexType,MemorySpace,cusp::csrb_format>
{
  typedef cusp::detail::binary_matrix_base<IndexType,MemorySpace,cusp::csrb_format> Parent;
  public:
    /*! rebind matrix to a different MemorySpace
     */
    template<typename MemorySpace2>
    struct rebind { typedef cusp::csrb_matrix<IndexType, MemorySpace2> type; };
    
    /*! type of row offsets indices array
     */
    typedef typename cusp::array1d<IndexType, MemorySpace> row_offsets_array_type;

    /*! type of column indices array
     */
    typedef typename cusp::array1d<IndexType, MemorySpace> column_indices_array_type;
    
    /*! equivalent container type
     */
    typedef typename cusp::csrb_matrix<IndexType, MemorySpace> container;

    /*! equivalent view type
     */
    typedef typename cusp::csrb_matrix_view<typename row_offsets_array_type::view,
                                           typename column_indices_array_type::view,
                                           IndexType, MemorySpace> view;
    
    /*! equivalent const_view type
     */
    typedef typename cusp::csrb_matrix_view<typename row_offsets_array_type::const_view,
                                           typename column_indices_array_type::const_view,
                                           IndexType, MemorySpace> const_view;
    
    /*! Storage for the row offsets of the CSR data structure.  Also called the "row pointer" array.
     */
    row_offsets_array_type row_offsets;
    
    /*! Storage for the column indices of the CSR data structure.
     */
    column_indices_array_type column_indices;

    /*! Construct an empty \p csrb_matrix.
     */
    csrb_matrix() {}

    /*! Construct a \p csrb_matrix with a specific shape and number of nonzero entries.
     *
     *  \param num_rows Number of rows.
     *  \param num_cols Number of columns.
     *  \param num_entries Number of nonzero matrix entries.
     */
    csrb_matrix(size_t num_rows, size_t num_cols, size_t num_entries)
      : Parent(num_rows, num_cols, num_entries),
        row_offsets(num_rows + 1), column_indices(num_entries) {}

    /*! Construct a \p csrb_matrix from another matrix.
     *
     *  \param matrix Another sparse or dense matrix.
     */
    template <typename MatrixType>
    csrb_matrix(const MatrixType& matrix);
    
    /*! Resize matrix dimensions and underlying storage
     */
    void resize(size_t num_rows, size_t num_cols, size_t num_entries)
    {
      Parent::resize(num_rows, num_cols, num_entries);
      row_offsets.resize(num_rows + 1);
      column_indices.resize(num_entries);
    }

    /*! Swap the contents of two \p csrb_matrix objects.
     *
     *  \param matrix Another \p csrb_matrix with the same IndexType and ValueType.
     */
    void swap(csrb_matrix& matrix)
    {
      Parent::swap(matrix);
      row_offsets.swap(matrix.row_offsets);
      column_indices.swap(matrix.column_indices);
    }
    
    /*! Assignment from another matrix.
     *
     *  \param matrix Another sparse or dense matrix.
     */
    template <typename MatrixType>
    csrb_matrix& operator=(const MatrixType& matrix);
}; // class csrb_matrix
/*! \}
 */

/*! \addtogroup sparse_matrix_views Sparse Matrix Views
 *  \ingroup sparse_matrices
 *  \{
 */

/*! \p csrb_matrix_view : Compressed Sparse Row matrix view
 *
 * \tparam Array1 Type of \c row_offsets array view
 * \tparam Array2 Type of \c column_indices array view
 * \tparam IndexType Type used for matrix indices (e.g. \c int).
 * \tparam MemorySpace A memory space (e.g. \c cusp::host_memory or cusp::device_memory)
 *
 */
template <typename Array1,
          typename Array2,
          typename IndexType   = typename Array1::value_type,
          typename MemorySpace = typename cusp::minimum_space<typename Array1::memory_space, typename Array2::memory_space>::type >
class csrb_matrix_view : public cusp::detail::binary_matrix_base<IndexType,MemorySpace,cusp::csrb_format>
{
  typedef cusp::detail::binary_matrix_base<IndexType,MemorySpace,cusp::csrb_format> Parent;
  public:
    typedef Array1 row_offsets_array_type;
    typedef Array2 column_indices_array_type;
    
    /*! equivalent container type
     */
    typedef typename cusp::csrb_matrix<IndexType, MemorySpace> container;

    /*! equivalent view type
     */
    typedef typename cusp::csrb_matrix_view<Array1, Array2, IndexType, MemorySpace> view;

    /*! Storage for the row offsets of the CSR data structure.  Also called the "row pointer" array.
     */
    row_offsets_array_type row_offsets;
    
    /*! Storage for the column indices of the CSR data structure.
     */
    column_indices_array_type column_indices;

    // construct empty view
    csrb_matrix_view(void)
      : Parent() {}

    // construct from existing CSR matrix or view
    template <typename Matrix>
    csrb_matrix_view(Matrix& A)
      : Parent(A),
        row_offsets(A.row_offsets),
        column_indices(A.column_indices) {}

    // TODO check sizes here
    csrb_matrix_view(size_t num_rows,
                    size_t num_cols,
                    size_t num_entries,
                    Array1 row_offsets,
                    Array2 column_indices)
      : Parent(num_rows, num_cols, num_entries),
        row_offsets(row_offsets),
        column_indices(column_indices) {}

    /*! Resize matrix dimensions and underlying storage
     */
    void resize(size_t num_rows, size_t num_cols, size_t num_entries)
    {
      Parent::resize(num_rows, num_cols, num_entries);
      row_offsets.resize(num_rows + 1);
      column_indices.resize(num_entries);
    }
};

/* Convenience functions */

template <typename Array1,
          typename Array2>
csrb_matrix_view<Array1,Array2>
make_csrb_matrix_view(size_t num_rows,
                     size_t num_cols,
                     size_t num_entries,
                     Array1 row_offsets,
                     Array2 column_indices)
{
  return csrb_matrix_view<Array1,Array2>
    (num_rows, num_cols, num_entries,
     row_offsets, column_indices);
}

template <typename Array1,
          typename Array2,
          typename IndexType,
          typename MemorySpace>
csrb_matrix_view<Array1,Array2,IndexType,MemorySpace>
make_csrb_matrix_view(const csrb_matrix_view<Array1,Array2,IndexType,MemorySpace>& m)
{
  return csrb_matrix_view<Array1,Array2,IndexType,MemorySpace>(m);
}
    
template <typename IndexType, class MemorySpace>
typename csrb_matrix<IndexType,MemorySpace>::view
make_csrb_matrix_view(csrb_matrix<IndexType,MemorySpace>& m)
{
  return make_csrb_matrix_view
    (m.num_rows, m.num_cols, m.num_entries,
     make_array1d_view(m.row_offsets),
     make_array1d_view(m.column_indices));
}

template <typename IndexType, class MemorySpace>
typename csrb_matrix<IndexType,MemorySpace>::const_view
make_csrb_matrix_view(const csrb_matrix<IndexType,MemorySpace>& m)
{
  return make_csrb_matrix_view
    (m.num_rows, m.num_cols, m.num_entries,
     make_array1d_view(m.row_offsets),
     make_array1d_view(m.column_indices));
}
/*! \}
 */

} // end namespace cusp

#include <cusp/detail/csrb_matrix.inl>

