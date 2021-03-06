#include "../../Block_Matrix.hxx"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

void read_free_var_matrix(const boost::filesystem::path &sdp_directory,
                          const std::vector<size_t> &block_indices,
                          const El::Grid &grid, Block_Matrix &free_var_matrix)
{
  free_var_matrix.blocks.reserve(block_indices.size());
  for(auto &block_index : block_indices)
    {
      const boost::filesystem::path free_var_matrix_path(
        sdp_directory / ("free_var_matrix." + std::to_string(block_index)));
      boost::filesystem::ifstream free_var_matrix_stream(free_var_matrix_path);
      if(!free_var_matrix_stream.good())
        {
          throw std::runtime_error("Could not open '"
                                   + free_var_matrix_path.string() + "'");
        }

      size_t height, width;
      free_var_matrix_stream >> height >> width;
      if(!free_var_matrix_stream.good())
        {
          throw std::runtime_error("Corrupted header in file: "
                                   + free_var_matrix_path.string());
        }
      free_var_matrix.blocks.emplace_back(height, width, grid);
      auto &block(free_var_matrix.blocks.back());
      for(size_t row = 0; row < height; ++row)
        for(size_t column = 0; column < width; ++column)
          {
            El::BigFloat input_num;
            free_var_matrix_stream >> input_num;
            if(block.IsLocal(row, column))
              {
                block.SetLocal(block.LocalRow(row), block.LocalCol(column),
                               input_num);
              }
          }
      if(!free_var_matrix_stream.good())
        {
          throw std::runtime_error("Corrupted data in file: "
                                   + free_var_matrix_path.string());
        }
    }
}
