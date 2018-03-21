#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>

class File
  {
  public:
    File();
    File(const char* name, const char* mode);

    virtual ~File();

    void open(const char* name, const char* mode);
    void close();

    void read(void* buff, size_t size_of_elem, size_t count);
    void write(const void* buff, size_t size_of_elem, size_t count);

    int seek(long offset, int origin);

    int get_length();

    bool is_open() const { return _is_open; }
    const char * const name() const { return _name; }

  private:
    char *_name;
    bool _is_open;
    FILE *p_file;

    char* _cpy_name(const char* src);
    bool _open(const char* name, const char* mode);
  };

File::File()
  : _name(0) {}

File::File(const char* name, const char* mode)
  {
  _name = _cpy_name(name);
  _is_open = _open(_name, mode);
  }

File::~File()
  {
  close();
  if (_name) free(_name);
  }

void File::open(const char* name, const char* mode)
  {
  _name = _cpy_name(name);
  _is_open = _open(_name, mode);
  }

void File::close()
  {
  if (p_file)
    {
    fclose(p_file);
    p_file = 0;
    }
  }

char* File::_cpy_name(const char* src)
  {
  char *name = (char*)malloc((strlen(src) + 1) * sizeof(char));
  assert(name);

  strcpy(name, src);
  return name;
  }

bool File::_open(const char* name, const char* mode)
  {
  p_file = fopen(name, mode);
  return p_file;
  }

void File::read(void* buff, size_t size_of_elem, size_t count)
  {
  fread(buff, size_of_elem, count, p_file);
  }

void File::write(const void* buff, size_t size_of_elem, size_t count)
  {
  fwrite(buff, size_of_elem, count, p_file);
  }

int File::seek(long offset, int origin)
  {
  return fseek(p_file, offset, origin);
  }

int File::get_length()
  {
  seek(0, SEEK_END);

  int len = ftell(p_file);
  rewind(p_file);

  return len;
  }

class GoodFile : public File
  {
  public:
    GoodFile() {}
    GoodFile(const char* name, const char* mode)
      : File(name, mode) {}

    //dont read bad things:)
    void read(char* buff, size_t count)
      {
      char *tmp = (char*)malloc((count + 1)* sizeof(char));

      memset(tmp, 0, count + 1);
      memset(buff, 0, count);

      File::read(tmp, sizeof(char), count);

      char *pos = strstr(tmp, "bad");
      if (!pos)
        {
        strcpy(buff, tmp);
        free(tmp);

        return;
        }

      int i_pos = pos - tmp;
      memcpy(buff, tmp, i_pos);

      char *s1 = buff + i_pos;
      char *s2 = tmp + i_pos + strlen("bad") + 1;

      memcpy(s1, s2, strlen(s2));

      free(tmp);
      }
  };

int main()
  {
  GoodFile file("file.txt", "w");

  if (!file.is_open())
    return 1;

  char str[] = "Hello bad world!";
  int len = strlen(str);

  file.write(str, sizeof(char), len);

  str[0] = 0;
  file.close();

  file.open("file.txt", "r");
  file.read(str, len);

  printf("%s", str);

  getchar();

  return 0;
  }
