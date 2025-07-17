// GenerateAst.cpp
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";
  return s.substr(start, end - start + 1);
}

// Helper function to capitalize the first letter of a string
std::string capitalize(const std::string &s) {
  if (s.empty())
    return s;
  std::string result = s;
  result[0] = std::toupper(result[0]);
  return result;
}

// Function to define the AST classes and write to a header file
void defineAst(const std::string &outputDir, const std::string &baseName,
               const std::vector<std::string> &types) {
  std::string path = outputDir + "/" + baseName + ".hpp";
  std::ofstream writer(path);
  if (!writer.is_open()) {
    std::cerr << "Could not open file: " << path << " for writing.\n";
    exit(1);
  }

  // Write header guard
  std::string guard = baseName + "_H";
  std::transform(guard.begin(), guard.end(), guard.begin(), ::toupper);
  writer << "#ifndef " << guard << "\n";
  writer << "#define " << guard << "\n\n";

  // Include necessary headers
  writer << "#include <memory>\n";
  writer << "#include <vector>\n";
  writer << "#include <any>\n";
  writer << "#include <string>\n";
  writer << "#include \"Token.hpp\"\n\n";
  writer << "\n";
  writer << "namespace Autumn {\n\n";
  for (const auto &type : types) {
    size_t colon = type.find(":");
    if (colon == std::string::npos) {
      std::cerr << "Invalid type definition: " << type << "\n";
      exit(1);
    }
    std::string className = trim(type.substr(0, colon));
    writer << "class " << className << ";" << std::endl;
  }
  writer << std::endl;

  // Abstract base class
  writer << "class " << baseName << " {\n";
  writer << "public:\n";
  writer << "    // Visitor interface\n";
  writer << "    class Visitor {\n";
  writer << "    public:\n";
  for (const auto &type : types) {
    std::string className = trim(type.substr(0, type.find(":")));
    className = capitalize(className);
    writer << "        virtual std::any visit" << className << baseName
           << "(std::shared_ptr<" << className << "> stmt) = 0;\n";
  }
  writer << "        virtual ~Visitor() = default;\n";
  writer << "    };\n\n";

  // Accept method
  writer << "    virtual std::any accept(Visitor& visitor) = 0;\n";
  writer << "    virtual ~" << baseName << "() = default;\n";
  writer << "};\n\n";

  // Define each type
  for (const auto &type : types) {
    size_t colon = type.find(":");
    if (colon == std::string::npos) {
      std::cerr << "Invalid type definition: " << type << "\n";
      exit(1);
    }
    std::string className = trim(type.substr(0, colon));
    std::string fields = trim(type.substr(colon + 1));

    className = capitalize(className);

    writer << "class " << className << " : public " << baseName
           << ", public std::enable_shared_from_this<" + className + "> {\n";
    writer << "public:\n";

    // Constructor
    writer << "    " << className << "(";
    std::vector<std::string> fieldList;
    size_t start = 0;
    size_t comma;
    while ((comma = fields.find(", ", start)) != std::string::npos) {
      fieldList.push_back(trim(fields.substr(start, comma - start)));
      start = comma + 2;
    }
    fieldList.push_back(trim(fields.substr(start)));
    for (size_t i = 0; i < fieldList.size(); ++i) {
      writer << fieldList[i];
      if (i < fieldList.size() - 1) {
        writer << ", ";
      }
    }
    writer << ") : ";

    // Initialize fields
    for (size_t i = 0; i < fieldList.size(); ++i) {
      std::string field = fieldList[i];
      size_t space = field.find_last_of(" ");
      if (space == std::string::npos) {
        std::cerr << "Invalid field definition: " << field << "\n";
        exit(1);
      }
      std::string typeStr = field.substr(0, space);
      std::string nameStr = field.substr(space + 1);
      writer << nameStr << "(" << nameStr << ")";
      if (i < fieldList.size() - 1) {
        writer << ", ";
      }
    }
    writer << " {}\n\n";

    // Override accept method
    writer << "    std::any accept(Visitor& visitor) override {\n";
    writer << "        return visitor.visit" << className << baseName
           << "(shared_from_this());\n";
    writer << "    }\n\n";

    // Fields
    for (const auto &field : fieldList) {
      writer << "    const " << field << ";\n";
    }

    writer << "};\n\n";
  }

  writer << "} // namespace Lox\n\n";
  writer << "#endif // " << guard << "\n";
  writer.close();

  std::cout << "Generated " << path << "\n";
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: GenerateAst <output directory>\n";
    return 64;
  }

  std::string outputDir = argv[1];

  // Define Expr types
  std::vector<std::string> exprTypes = {
      "Assign   : Token name, std::shared_ptr<Expr> value",
      "Binary   : std::shared_ptr<Expr> left, Token op, "
      "std::shared_ptr<Expr> right",
      "Call     : std::shared_ptr<Expr> callee, "
      "std::vector<std::shared_ptr<Expr>> "
      "arguments",
      "Get      : std::shared_ptr<Expr> object, Token name",
      "Grouping : std::shared_ptr<Expr> expression",
      "Literal  : std::any value",
      "Logical  : std::shared_ptr<Expr> left, Token op, "
      "std::shared_ptr<Expr> right",
      "Set      : std::shared_ptr<Expr> object, Token name, "
      "std::shared_ptr<Expr> value",
      "Unary    : Token op, std::shared_ptr<Expr> right",
      "Lambda   : std::vector<Token> params, std::shared_ptr<Expr> right",
      "Variable : Token name",
      "TypeVariable : Token name",
      "TypeDecl : Token name, std::shared_ptr<Expr> typeexpr",
      "ListTypeExpr : std::shared_ptr<Expr> typeexpr",
      "ListVarExpr : std::vector<std::shared_ptr<Expr>> varExprs",
      "IfExpr         : std::shared_ptr<Expr> condition, "
      "std::shared_ptr<Expr> thenBranch, std::shared_ptr<Expr> elseBranch",
      "Let: std::vector<std::shared_ptr<Expr>> exprs",
      "InitNext: std::shared_ptr<Expr> initializer, "
      "std::shared_ptr<Expr> nextExpr"};

  // Define Stmt types
  std::vector<std::string> stmtTypes = {
      "Block      : std::vector<std::shared_ptr<Stmt>> statements",
      "Object     : Token name, std::vector<std::shared_ptr<Expr>> fields, "
      "std::shared_ptr<Expr> Cell",
      "Expression : std::shared_ptr<Expr> expression",
      "OnStmt : std::shared_ptr<Expr> condition, std::shared_ptr<Expr> expr"};
  /// DONT HAVE explicit var stmt, instead, the first assignment is the
  /// declaration
  // Generate Expr.h
  defineAst(outputDir, "Expr", exprTypes);

  // Generate Stmt.h
  defineAst(outputDir, "Stmt", stmtTypes);

  return 0;
}
