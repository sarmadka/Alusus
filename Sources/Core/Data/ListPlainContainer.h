/**
 * @file Core/Data/ListPlainContainer.h
 * Contains the header of interface Data::ListPlainContainer.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_LISTPLAINCONTAINER_H
#define DATA_LISTPLAINCONTAINER_H

namespace Core { namespace Data
{

// TODO: DOC

class ListPlainContainer : public virtual PlainContainer
{
  //============================================================================
  // Type Info

  INTERFACE_INFO(ListPlainContainer, PlainContainer, "Core.Data", "Core", "alusus.net");


  //============================================================================
  // Abstract Functions

  public: virtual void add(IdentifiableObject *obj) = 0;

}; // class

} } // namespace

#endif
