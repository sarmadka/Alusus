/**
 * @file Spp/Ast/CastOp.h
 * Contains the header of class Spp::Ast::CastOp.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_AST_CASTOP_H
#define SPP_AST_CASTOP_H

namespace Spp::Ast
{

class CastOp : public Core::Data::Node,
               public Binding, public MapContaining<TiObject>,
               public Core::Data::Ast::MetaHaving, public Core::Data::Clonable,
               public Core::Data::Printable
{
  //============================================================================
  // Type Info

  TYPE_INFO(CastOp, Core::Data::Node, "Spp.Ast", "Spp", "alusus.org");
  IMPLEMENT_INTERFACES(Core::Data::Node, Binding, MapContaining<TiObject>,
                                         Core::Data::Ast::MetaHaving, Core::Data::Clonable, Core::Data::Printable);


  //============================================================================
  // Member Variables

  private: TioSharedPtr operand;
  private: TioSharedPtr targetType;


  //============================================================================
  // Implementations

  IMPLEMENT_METAHAVING(CastOp);

  IMPLEMENT_BINDING(Binding,
    (prodId, TiWord, VALUE, setProdId(value), &prodId),
    (sourceLocation, Core::Data::SourceLocation, SHARED_REF, setSourceLocation(value), sourceLocation.get())
  );

  IMPLEMENT_MAP_CONTAINING(MapContaining<TiObject>,
    (operand, TiObject, SHARED_REF, setOperand(value), operand.get()),
    (targetType, TiObject, SHARED_REF, setTargetType(value), targetType.get())
  );

  IMPLEMENT_AST_CLONABLE(PointerOp);

  IMPLEMENT_AST_MAP_PRINTABLE(CastOp);


  //============================================================================
  // Constructors & Destructor

  IMPLEMENT_EMPTY_CONSTRUCTOR(CastOp);

  IMPLEMENT_ATTR_CONSTRUCTOR(CastOp);

  IMPLEMENT_ATTR_MAP_CONSTRUCTOR(CastOp);

  public: virtual ~CastOp()
  {
    DISOWN_SHAREDPTR(this->operand);
    DISOWN_SHAREDPTR(this->targetType);
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

  public: void setTargetType(TioSharedPtr const &t)
  {
    UPDATE_OWNED_SHAREDPTR(this->targetType, t);
  }
  private: void setTargetType(TiObject *t)
  {
    this->setTargetType(getSharedPtr(t));
  }

  public: TioSharedPtr const& getTargetType() const
  {
    return this->targetType;
  }

}; // class

} // namespace

#endif
