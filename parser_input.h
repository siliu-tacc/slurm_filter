#include<string>
#include<memory>

#ifndef PARSER_INPUT
#define PARSER_INPUT

class Parser_Input {

 public:
  Parser_Input  ();
  ~Parser_Input  ();

  /** 
   * \ingroup CXXinput
   * \name File open/close:
   *
   */

  /**
   * \ingroup CXXinput
   * @{ 
   */

  /// \brief open \e filename for parsing
  int  Open         (const char *filename);

  /// \brief close previously opened input file.
  int  Close        ();

  /** @} */

/** \ingroup CXXinput
   *  \name Dump input file contents:
   * 
   */

  /** \ingroup CXXinput
   * @{
   */

  /** \brief Dump input file settings to stdout. */
  int  Fdump        ();

  /** \brief Dump input file settings to stdout prefixed by a delimiter */
  int  Fdump        (const char *prefix);

  /** \brief Dump input file settings to an open file with a provided prefix delimiter */
  int  Fdump        (const char *prefix, const char *filename);

  /** @} */

  /** \ingroup CXXinput
   *  \name Read scalar values
   *
* 
   * The following member functions are used to read a scalar input \e
   * value associated with the \e keyword string. The \e keyword can
   * be provided as a string or const char* pointer. If desired, a \e
   * vardef argument can be included which provides a default value
   * for the scalar if the requested \e keyword is not present in the
   * input file.. Alternatively, you can use the Register_Var family
   * of functions to provide default values.  Supported numeric template 
   * datatypes include int, float, and double.
   */

  /** \ingroup CXXinput
   * @{
   */

  template <typename T> int Read_Var      (std::string keyword, T *value );           ///< Scalar read
  template <typename T> int Read_Var      (std::string keyword, T *value, T vardef);  ///< Scalar read with default

  template <typename T> int Read_Var      (const char *keyword, T *value );           ///< Scalar read
  template <typename T> int Read_Var      (const char *keyword, T *value, T vardef);  ///< Scalar read with default

  // Character string reads

  int Read_Var                            (const char *keyword, std::string *value);                     ///< Scalar
  int Read_Var                            (const char *keyword, char **value);                           ///< Scalar
  int Read_Var                            (const char *keyword, std::string *value, std::string vardef); ///< Scalar 

  // Bool Read Functions

  int Read_Var                            (const char *var, bool *value, bool vardef); ///< Scalar
  int Read_Var                            (std::string var, bool *value, bool vardef); ///< Scalar

 /** @} */

  /** \ingroup CXXinput
   *  \name Read vector values 
   * 
   * The following member functions are used to read vector valued
   * input values for common intrinsic datatypes given a desired
   * \e keyword. If successful, the resulting
   * data is stored in the array values.
   */

  /** \ingroup CXXinput
   * @{
   */

  template <typename T> int Read_Var_Vec  (std::string keyword, T *values, int nelems);            ///< Vec
  template <typename T> int Read_Var_Vec  (const char *keyword, T *values, int nelems);            ///< Vec
  template <typename T> int Read_Var_Vec  (std::string keyword, T *values, int nelems, T vardef);  ///< Vec
  template <typename T> int Read_Var_Vec  (const char *keyword, T *values, int nelems, T vardef);  ///< Vec

  /** @} */

  /** \ingroup CXXinput
   *  \name Read ith vector value 
   * 
   * The following member functions are used to read the ith entry (\e elem) of 
   * vector valued input values for common intrinsic datatypes given a desired 
   * keyword. If successful, the resulting
   * data is stored in \e value.
   */

  /** \ingroup CXXinput
   * @{
   */
template <typename T> int Read_Var_iVec (std::string keyword, T *value, int elem);               ///< iVec
  template <typename T> int Read_Var_iVec (const char *keyword, T *value, int elem);               ///< iVec
  template <typename T> int Read_Var_iVec (std::string keyword, T *value, int elem,   T vardef);   ///< iVec
  template <typename T> int Read_Var_iVec (const char *keyword, T *value, int elem,   T vardef);   ///< iVec

  // Character String Read Functions

  int Read_Var_iVec                       (const char *keyword, char **value, int elem);           ///<iVec
  int Read_Var_iVec                       (const char *keyword, std::string *value, int elem);     ///<iVec

  /** @} */

  /** \ingroup CXXinput
   *  \name Default value registration:
   * 
   * The following member functions provide a mechanism to register a
   * default value with the input parsing mechanism. If a particular
   * \e keyword has a registered default value and is subsequently
   * queried with an input file which does not contain the keyword,
   * the registered default value will be returned instead. A
   * convenient use for these registration functions is to provide
   * sensible defaults to newly added input keywords which are not
   * present in older input file revisions.
   */

  /** \ingroup CXXinput
   * @{
   */

void Register_Var  (const char *keyword, int     var);           ///< Register_Var
  void Register_Var  (const char *keyword, float   var);           ///< Register_Var
  void Register_Var  (const char *keyword, double  var);           ///< Register_Var
  void Register_Var  (const char *keyword, bool    var);           ///< Register_Var
  void Register_Var  (const char *keyword, const char  *var);      ///< Register_Var
  void Register_Var  (const char *keyword, std::string  var);      ///< Register_Var

  /** @} */

  /** 
   * \ingroup CXXinput 
   * \name Default registration query:
   *
   * The following member functions can be used to query the current
   * hashed values of any keyword variables which have been registered
   * previously.
   */

  /** \ingroup CXXinput
   * @{
   */

  int  Get_Var       (const char *keyword, int    *value);       ///< Get_Var
  int  Get_Var       (const char *keyword, float  *value);       ///< Get_Var
  int  Get_Var       (const char *keyword, double *value);       ///< Get_Var
  int  Get_Var       (const char *keyword, char  **value);       ///< Get_Var
  int  Get_Var       (const char *keyword, std::string *value);  ///< Get_Var

  void PrintRegVars  (const char *prefix); ///< Print

  /** @} */

 private:
  class Parser_InputImp       ;           // forward declaration to Input class implementation
  std::auto_ptr<Parser_InputImp> m_pimpl; // pointer to implementation

};

#endif
