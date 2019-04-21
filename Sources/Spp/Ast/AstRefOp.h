/**
 * @file Spp/Ast/AstRefOp.h
 * Contains the header of class Spp::Ast::AstRefOp.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_ASTREFOP_H
#define SPP_AST_ASTREFOP_H

namespace Spp::Ast
{

class AstRefOp : public Core::Data::Node,
                 public virtual Binding, public virtual MapContaining<TiObject>,
                 public virtual Core::Data::Ast::MetaHaving, public virtual Core::Data::Clonable,
                 public virtual Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(AstRefOp, Core::Data::Node, "Spp.Ast", "Spp", "alusus.net");
  IMPLEMENT_INTERFACES(Core::Data::Node, Binding, MapContaining<TiObject>,
                                         Core::Data::Ast::MetaHaving, Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr operand;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(AstRefOp);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>, (operand, TiObject, SHARED_REF, setOperand(value), operand.get()));

  IMPLEMENT_AST_CLONABLE(AstRefOp);

  IMPLEMENT_AST_MAP_PRINTABLE(AstRefOp);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(AstRefOp);

  IMPLEMENT_ATTR_CONSTRUCTOR(AstRefOp);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(AstRefOp);

  public: virtual ~AstRefOp()
  {
    DISOWN_SHAREDPTR(this->operand);
  }


  //============================================================================
  // Member Functions

  public: void setOperand(TioSharedPtr const &o)
  {
    UPDATE_OWNED_SHAREDPTR(this->operand, o);
  }
  private: void setOperand(TiObject *o)
  {
    this->setOperand(getSharedPtr(o));
  }

  public: TioSharedPtr const& getOperand() const
  {
    return this->operand;
  }

}; // class

} // namespace

#endif
