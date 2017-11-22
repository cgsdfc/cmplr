#include <cstddef>
#include <cstdio>
#include <utility>

extern "C" {
#include <utillib/scanner.h>
}

namespace utillib {
class Scanner {
public:
  virtual std::size_t Lookahead() = 0;
  virtual void Shiftaway() = 0;
  virtual void const *Semantic() = 0;
  virtual bool Reacheof() = 0;
  virtual struct utillib_scanner_op const *GetOp() = 0;
};

class SymbolScanner final : public Scanner {
public:
  SymbolScanner(const size_t *symbols,
                struct utillib_symbol const *table) noexcept;
  std::size_t Lookahead() override;
  void Shiftaway() override;
  void const *Semantic() override;
  bool Reacheof() override;
  struct utillib_scanner_op const *GetOp() override;

private:
  struct utillib_symbol_scanner scanner_;
};

class CharScanner final : public Scanner {
public:
  CharScanner(std::FILE *file);
  ~CharScanner() noexcept;
  std::size_t Lookahead() override;
  void Shiftaway() override;
  void const *Semantic() override;
  bool Reacheof() override;
  struct utillib_scanner_op const *GetOp() override;
  std::pair<std::size_t, std::size_t> GetLocation() const;

private:
  struct utillib_char_scanner scanner_;
};

class TokenScannerError {};

class TokenScannerCore {
public:
  virtual int Read() = 0;
  virtual int Recover() = 0;
  virtual void const *Semantic(int value, char const *string) = 0;
};

class TokenScanner final : public Scanner {};
} // namespace
