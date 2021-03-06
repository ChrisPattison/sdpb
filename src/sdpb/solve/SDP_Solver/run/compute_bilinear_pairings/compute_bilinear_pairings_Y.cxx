#include "../../../Block_Diagonal_Matrix.hxx"

// bilinear_pairings_Y[b] = Q[b]'^T A[b] Q[b]' for each block 0 <= b < Q.size()
// bilinear_pairings_Y[b], A[b] denote the b-th blocks of bilinear_pairings_Y,
// A, resp.

// Q[b]' = Q[b] \otimes 1, where \otimes denotes tensor product

// for each b, L.blocks[b], Q[b], Work[b], and bilinear_pairings_Y.blocks[b]
// must have the structure described above for `tensorTransposeCongruence'

void compute_bilinear_pairings_Y(
  const Block_Diagonal_Matrix &Y,
  const std::vector<El::Matrix<El::BigFloat>> &bilinear_bases,
  std::vector<El::DistMatrix<El::BigFloat>> &workspace,
  Block_Diagonal_Matrix &bilinear_pairings_Y)
{
  auto Y_block(Y.blocks.begin());
  auto bilinear_pairings_Y_block(bilinear_pairings_Y.blocks.begin());
  auto bilinear_bases_block(bilinear_bases.begin());

  for(auto &work : workspace)
    {
      // FIXME: This should be a constant, not computed over and over.
      // Set up the workspace to have copies of bilinear_bases
      // along the diagonal
      for(int64_t row = 0; row < work.LocalHeight(); ++row)
        {
          size_t global_row(work.GlobalRow(row)),
            row_block(global_row / bilinear_bases_block->Height());

          for(int64_t column = 0; column < work.LocalWidth(); ++column)
            {
              size_t global_column(work.GlobalCol(column)),
                column_block(global_column / bilinear_bases_block->Width());
              work.SetLocal(
                row, column,
                row_block != column_block
                  ? El::BigFloat(0)
                  : (*bilinear_bases_block)(
                      global_row % bilinear_bases_block->Height(),
                      global_column % bilinear_bases_block->Width()));
            }
        }
      auto temp_space(work);
      Gemm(El::Orientation::NORMAL, El::Orientation::NORMAL, El::BigFloat(1),
           *Y_block, work, El::BigFloat(0), temp_space);
      Gemm(El::Orientation::TRANSPOSE, El::Orientation::NORMAL,
           El::BigFloat(1), work, temp_space, El::BigFloat(0),
           *bilinear_pairings_Y_block);
      El::MakeSymmetric(El::UpperOrLower::LOWER, *bilinear_pairings_Y_block);
      ++Y_block;
      ++bilinear_pairings_Y_block;
      ++bilinear_bases_block;
    }
}
