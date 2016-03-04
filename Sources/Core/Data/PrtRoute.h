/**
 * @file Core/Data/PrtRoute.h
 * Contains the header of class Core::Data::PrtRoute.
 *
 * @copyright Copyright (C) 2015 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_PRTROUTE_H
#define DATA_PRTROUTE_H

namespace Core { namespace Data
{

// TODO: DOC

/**
 * @brief Contains parsed information of a single route in a PRT.
 * @ingroup data
 *
 * A decision made at an alternative or optional term is recorded in this object
 * by the GenericParsingHandler along with the child data resulting from
 * taking that route. This object is created by the GenericParsingHandler to
 * compose the Parsing Representation Tree (PRT).
 */
class PrtRoute : public Node,
                 public virtual ParsingMetadataHolder, public virtual Container
{
  //============================================================================
  // Type Info

  TYPE_INFO(PrtRoute, Node, "Core.Data", "Core", "alusus.net");
  IMPLEMENT_INTERFACES_2(Node, ParsingMetadataHolder, Container);


  //============================================================================
  // Member Variables

  /**
   * @brief A parameter representing the parsing decision made.
   *
   * The value of this parameter gives an indication to the parsing decision
   * made on the term. The value depends on the type of the term.
   * Alternative Term: For alternative terms this value refers to the index
   *                   of the selected term/path.
   * Optional Term: For optional terms this value can be 0 or 1.
   */
  public: Int route;

  /**
   * @brief The child parsed data for the term.
   *
   * The parsed data from the terms contained in the selected route (if any)
   * is recorded to this pointer.
   */
  public: SharedPtr<IdentifiableObject> data;


  //============================================================================
  // Constructor / Destructor

  public: PrtRoute(Word pid=UNKNOWN_ID, Int r=-1, IdentifiableObject *d=0) :
    ParsingMetadataHolder(pid), route(r), data(d)
  {
    OWN_SHAREDPTR(this->data);
  }

  public: PrtRoute(Word pid, Int r, SharedPtr<IdentifiableObject> const &d) :
    ParsingMetadataHolder(pid), route(r), data(d)
  {
    OWN_SHAREDPTR(this->data);
  }

  public: PrtRoute(Word pid, Int r, SourceLocation const &sl, IdentifiableObject *d=0) :
    ParsingMetadataHolder(pid, sl), route(r), data(d)
  {
    OWN_SHAREDPTR(this->data);
  }

  public: PrtRoute(Word pid, Int r, SourceLocation const &sl, SharedPtr<IdentifiableObject> const &d) :
    ParsingMetadataHolder(pid, sl), route(r), data(d)
  {
    OWN_SHAREDPTR(this->data);
  }

  public: virtual ~PrtRoute()
  {
    DISOWN_SHAREDPTR(this->data);
  }

  public: static SharedPtr<PrtRoute> create(Word pid=UNKNOWN_ID, Int r=-1, IdentifiableObject *d=0)
  {
    return std::make_shared<PrtRoute>(pid, r, d);
  }

  public: static SharedPtr<PrtRoute> create(Word pid, Int r, SharedPtr<IdentifiableObject> const &d)
  {
    return std::make_shared<PrtRoute>(pid, r, d);
  }

  public: static SharedPtr<PrtRoute> create(Word pid, Int r, SourceLocation const &sl, IdentifiableObject *d=0)
  {
    return std::make_shared<PrtRoute>(pid, r, sl, d);
  }

  public: static SharedPtr<PrtRoute> create(Word pid, Int r, SourceLocation const &sl,
                                               SharedPtr<IdentifiableObject> const &d)
  {
    return std::make_shared<PrtRoute>(pid, r, sl, d);
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the route index.
   *
   * The value of this parameter gives an indication to the parsing decision
   * made on the term. The value depends on the type of the term.
   * Alternative Term: For alternative terms this value refers to the index
   *                   of the selected term/path.
   * Optional Term: For optional terms this value can be 0 or 1.
   */
  public: void setRoute(Int r)
  {
    this->route = r;
  }

  /**
   * @brief Get the route index.
   *
   * The value of this parameter gives an indication to the parsing decision
   * made on the term. The value depends on the type of the term.
   * Alternative Term: For alternative terms this value refers to the index
   *                   of the selected term/path.
   * Optional Term: For optional terms this value can be 0 or 1.
   */
  public: Int getRoute() const
  {
    return this->route;
  }

  /**
   * @brief Set the child parsed data.
   *
   * This is the parsed data from the terms contained in the selected route
   * (if any) is recorded to this pointer.
   */
  public: void setData(SharedPtr<IdentifiableObject> const &d)
  {
    UPDATE_OWNED_SHAREDPTR(this->data, d);
  }

  /**
   * @brief Get a smart pointer to the child parsed data.
   *
   * This is the parsed data from the terms contained in the selected route
   * (if any) is recorded to this pointer.
   */
  public: SharedPtr<IdentifiableObject> const& getData() const
  {
    return this->data;
  }


  //============================================================================
  // ParsingMetadataHolder Overrides

  /**
   * @brief Override the original implementation to search the tree if needed.
   * If the value is not yet set for this object and it has children, it will
   * call getSourceLocation on the children.
   */
  public: virtual SourceLocation const& getSourceLocation() const
  {
    if (ParsingMetadataHolder::getSourceLocation().line == 0) {
      ParsingMetadataHolder *ptr = ii_cast<ParsingMetadataHolder>(this->getData().get());
      if (ptr != 0) {
        SourceLocation const &sl = ptr->getSourceLocation();
        if (sl.line != 0) return sl;
      }
    }
    return ParsingMetadataHolder::getSourceLocation();
  }

  public: virtual IdentifiableObject* getAttribute(Char const *name)
  {
    if (SBSTR(name) == STR("sourceLocation")) {
      if (ParsingMetadataHolder::getSourceLocation().line == 0) {
        ParsingMetadataHolder *ptr = ii_cast<ParsingMetadataHolder>(this->getData().get());
        if (ptr != 0) {
          return ptr->getAttribute(name);
        }
      }
    }
    return ParsingMetadataHolder::getAttribute(name);
  }


  //============================================================================
  // Container Implementation

  /// Change the element at the specified index.
  public: virtual void set(Int index, IdentifiableObject *val)
  {
    if (index == 0) {
      UPDATE_OWNED_SHAREDPTR(this->data, getSharedPtr(val, true));
    } else {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0 for this class."));
    }
  }

  /// Remove the element at the specified index.
  public: virtual void remove(Int index)
  {
    if (index != 0) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0 for this class."));
    }
    RESET_OWNED_SHAREDPTR(this->data);
  }

  /// Get the count of elements in the list.
  public: virtual Word getCount() const
  {
    return 1;
  }

  /// Get the object at the specified index.
  public: virtual IdentifiableObject* get(Int index) const
  {
    if (index != 0) {
      throw EXCEPTION(InvalidArgumentException, STR("index"), STR("Must be 0 for this class."));
    }
    return this->data.get();
  }

}; // class

} } // namespace

#endif