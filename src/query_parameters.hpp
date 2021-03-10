#ifndef __QPPH_H__
#define __QPPH_H__

class QueryParametersProvider {
  public:
    virtual ~QueryParametersProvider() {}
    virtual float getCutoff() = 0;
    virtual bool isCanon() = 0;
};

class QueryParameters: virtual public QueryParametersProvider {
  private:
    float m_cutoff;
    bool m_canonical;

  public: 
    QueryParameters(float cutoff, bool canonical) {
      m_cutoff = cutoff;
      m_canonical = canonical;
    }

  float getCutoff() {
    return m_cutoff;
  }


  bool isCanon() {
    return m_canonical;
  }
    
  
};

#endif