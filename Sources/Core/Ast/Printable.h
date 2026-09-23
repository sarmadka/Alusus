/**
 * @file Core/Ast/Printable.h
 * Contains the header of interface Core::Ast::Printable.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_PRINTABLE_H
#define CORE_AST_PRINTABLE_H

namespace Core::Ast
{

// TODO: DOC

class Printable : public TiInterface
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(Printable, TiInterface, "Core.Ast", "Core", "alusus.org");


  //============================================================================
  // Abstract Functions

  public: virtual void print(OutStream &stream, Int indents=0) const = 0;

  public: virtual Str toString(Int indents=0) const
  {
    StrStream stream;
    this->print(stream, indents);
    return stream.str().c_str();
  }

}; // class

} // namespace

#endif
