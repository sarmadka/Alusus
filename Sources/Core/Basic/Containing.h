/**
 * @file Core/Basic/Containing.h
 * Contains the header of interface Basic::Containing.
 *
 * @copyright Copyright (C) 2017 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_BASIC_CONTAINING_H
#define CORE_BASIC_CONTAINING_H

namespace Core::Basic
{

template<class CTYPE> class Containing : public TiInterface
{
  //============================================================================
  // Type Info

  TEMPLATE_INTERFACE_INFO(Containing, TiInterface, "Core.Basic", "Core", "alusus.net", (CTYPE));


  //============================================================================
  // Abstract Functions

  public: virtual void setElement(Int index, CTYPE *val) = 0;

  public: virtual void removeElement(Int index) = 0;

  public: virtual Word getElementCount() const = 0;

  public: virtual CTYPE* getElement(Int index) const = 0;

}; // class

} // namespace

#endif