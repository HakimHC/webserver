#ifndef __LOCATION_HPP__
# define __LOCATION_HPP__

# include <cstddef>
# include <string>
# include <vector>

# define DEFAULT_ROOT "www"
# define DEFAULT_INDEX "index.html"

class Location {
 public:
   Location();
   ~Location();
   Location(std::string &text, std::string &uri);


  /* Getters */
  const std::string&                getUri() const;
  const std::string&                getRoot() const;
  const std::string&                getIndex() const;
  const std::vector<std::string>&   getAllowedMethods() const;
  const size_t&                     getMaxClientBodySize() const;
  const std::string&                getDefaultFileDirectory() const;
  const std::string&                getRedirect() const;
  const bool&                       getListing() const;

  /* Setters */
  void                              setUri(const std::string&);
  void                              setRoot(const std::string&);
  void                              setIndex(const std::string&);
  void                              setAllowedMethods(const std::vector<std::string>&);
  void                              setMaxClientBodySize(const size_t&);
  void                              setDefaultFileDirectory(const std::string&);
  void                              setRedirect(const std::string&);
  void                              setListing(const bool&);

 static void 						removeTrailing(std::string &);		

 private:
  std::string                _uri;
  std::string                _root;
  std::string                _index;
  std::vector<std::string>   _allowedMethods;
  size_t                     _maxClientBodySize;
  std::string                _defaultFileDirectory;
  std::string                _redirect;
  bool                       _listing;
  void						 _setPriv(std::string line);
};

#endif // __LOCATION_HPP__
