#ifndef __QPPH_H__
#define __QPPH_H__

class QueryParametersProvider {
  public:
    virtual ~QueryParametersProvider() {}
    virtual float getCutoff() = 0;
    virtual bool isCanon() = 0;
    virtual char* getOutputFile() = 0;
    virtual bool shouldGzip() = 0;
};

class QueryParameters: virtual public QueryParametersProvider {
  private:
    float m_cutoff;
    bool m_canonical;
    std::string m_output_file;

  public: 
    QueryParameters(float cutoff, bool canonical, std::string output_file) {
      m_cutoff = cutoff;
      m_canonical = canonical;
      m_output_file = output_file;
    }

  float getCutoff() {
    return m_cutoff;
  }


  bool shouldGzip() {
    std::string gzipEnding = ".gz";
    return m_output_file.size() >= gzipEnding.size() && 0 == m_output_file.compare(m_output_file.size()-gzipEnding.size(), gzipEnding.size(), gzipEnding);
  }

  char * getOutputFile() {
    char * ofAsCharArray = new char [m_output_file.length()+1];
    strcpy (ofAsCharArray, m_output_file.c_str());
    return ofAsCharArray;
  }

  bool isCanon() {
    return m_canonical;
  }
    
  
};

#endif