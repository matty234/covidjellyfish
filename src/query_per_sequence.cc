#include <vector>
#include <iostream>
#include <string>

#include <jellyfish/err.hpp>
#include <jellyfish/thread_exec.hpp>
#include <jellyfish/file_header.hpp>
#include <jellyfish/stream_manager.hpp>
#include <jellyfish/whole_sequence_parser.hpp>
#include <jellyfish/mer_dna_bloom_counter.hpp>
#include <jellyfish/jellyfish.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include "sequence_mers.hpp"
#include "query_parameters.hpp"
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/iostreams/copy.hpp>


namespace err = jellyfish::err;
namespace boostio = boost::iostreams;

using jellyfish::mer_dna;
using jellyfish::mer_dna_bloom_counter;

typedef jellyfish::whole_sequence_parser<jellyfish::stream_manager<char **>> sequence_parser;

const float DEFAULT_CUTOFF = 0.6;

template <typename PathIterator, typename Database>
void query_from_sequence(PathIterator file_begin, const Database &db, QueryParametersProvider *qpp)
{

  FILE *fp = std::fopen(qpp->getOutputFile(), "w");
  const int fd = fileno(fp);

  PathIterator fbegin = file_begin;
  PathIterator fend = file_begin + 1;

  jellyfish::stream_manager<PathIterator> streams(fbegin, fend);

  sequence_parser parser(4, 100, 1, streams);
  sequence_mers mers(qpp->isCanon());
  const sequence_mers mers_end(qpp->isCanon());

  const float cutoff = qpp->getCutoff();

  boostio::file_descriptor_sink output(fd, boostio::never_close_handle);
  boostio::filtering_streambuf<boostio::output> out;

  out.push(boost::iostreams::newline_filter(boostio::newline::dos));
  out.push(boostio::gzip_compressor(boostio::gzip_params(boostio::gzip::best_compression)));
  out.push(output);

  std::ostream buffer(&out);

  while (true)
  {
    sequence_parser::job j(parser);
    if (j.is_empty())
      break;
    for (size_t i = 0; i < j->nb_filled; ++i)
    {
      mers = j->data[i].seq;

      int total = 0, len = 0;
      if (mers != mers_end)
      {
        uint32_t found = db.check(*mers);
        total = total + found;
        ++len;
        ++mers;
      }
      for (; mers != mers_end; ++mers)
      {
        uint32_t found = db.check(*mers);
        total = total + found;
        ++len;
      }

      int32_t size = j->data[i].seq.size();
      float_t val = (float)total / (float)size;

      if (val > cutoff)
      {
        buffer << "@" << j->data[i].header << std::endl;
        buffer << j->data[i].seq << std::endl;
        buffer << "+" << std::endl;
        buffer << j->data[i].qual << std::endl;
      }
    }
  }
  //output.close();
}

int main(int argc, char *argv[])
{
  float givenCutoff = DEFAULT_CUTOFF;

  if (argc == 4)
    ;
  else if (argc == 5)
    givenCutoff = atof(argv[5]);
  else
    err::die(err::msg() << "Usage: " << argv[0] << " db.jf file.fa output.fa cutoff");

  char *outputFile(argv[3]);
  char *inputFile(argv[3]);

  std::ifstream in(inputFile, std::ios::in | std::ios::binary);

  jellyfish::file_header header(in);
  if (!in.good())
    err::die(err::msg() << "Failed to parse header of file '" << argv[1] << "'");

  // Setup canon and cutoff
  QueryParameters qp(header.canonical(), givenCutoff, outputFile);

  // Setup k-mer length
  mer_dna::k(header.key_len() / 2);

  if (header.format() == "bloomcounter")
    err::die(err::msg() << "not implemented");

  jellyfish::mapped_file binary_map(inputFile);

  binary_query bq(binary_map.base() + header.offset(), header.key_len(), header.counter_len(), header.matrix(),
                  header.size() - 1, binary_map.length() - header.offset());

  query_from_sequence(argv + 2, bq, &qp);

  return 0;
}