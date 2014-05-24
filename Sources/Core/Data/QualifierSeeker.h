/**
 * @file Core/Data/QualifierSeeker.h
 * Contains the header of class Core::Data::QualifierSeeker.
 *
 * @copyright Copyright (C) 2014 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#ifndef DATA_QUALIFIERSEEKER_H
#define DATA_QUALIFIERSEEKER_H

namespace Core { namespace Data
{

// TODO: DOC

class QualifierSeeker
{
  //============================================================================
  // Member Variables

  private: const Provider *dataProvider;
  private: mutable ReferenceParser parser;


  //============================================================================
  // Constructor & Destructor

  public: QualifierSeeker(const Provider *prov=0) : dataProvider(prov)
  {
  }

  public: ~QualifierSeeker()
  {
  }


  //============================================================================
  // Member Functions

  /// @name Initialization Functions
  /// @{

  public: void setDataProvider(const Provider *prov)
  {
    this->dataProvider = prov;
  }

  public: const Provider* getDataProvider() const
  {
    return this->dataProvider;
  }

  /// @}

  /// @name Helper Functions
  /// @{

  private: Bool getImmediateContainer(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
                                      ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                      SharedPtr<Module> &retModule) const;

  private: Bool getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
                                      ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                      SharedPtr<Module> &retModule) const;

  private: Bool getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
                                      ReferenceSegment *&retSeg, IdentifiableObject *&retParent,
                                      Module *&retModule) const;

  private: Bool getImmediateContainer(const Char *qualifier, IdentifiableObject *parent,
                                      ReferenceSegment *&retSeg, IdentifiableObject *&retParent) const;

  /// @}

  /// @name Data Read Functions
  /// @{

  public: const SharedPtr<IdentifiableObject>& getShared(const Char *qualifier, IdentifiableObject *parent) const;

  public: Bool tryGetShared(const Char *qualifier, IdentifiableObject *parent,
                            SharedPtr<IdentifiableObject> &result) const;

  public: void getShared(const Char *qualifier, IdentifiableObject *parent,
                         SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const;

  public: void getShared(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
                         SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const;

  public: Bool tryGetShared(const Char *qualifier, IdentifiableObject *parent,
                            SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const;

  public: Bool tryGetShared(const Char *qualifier, const SharedPtr<IdentifiableObject> &parent,
                            SharedPtr<IdentifiableObject> &retVal, SharedPtr<Module> &retModule) const;

  /// @}

  /// @name Plain Data Read Functions
  /// @{

  public: IdentifiableObject* getPlain(const Char *qualifier, IdentifiableObject *parent) const;

  public: Bool tryGetPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *&result) const;

  public: void getPlain(const Char *qualifier, IdentifiableObject *parent,
                        IdentifiableObject *&retVal, Module *&retModule) const;

  public: Bool tryGetPlain(const Char *qualifier, IdentifiableObject *parent,
                           IdentifiableObject *&retVal, Module *&retModule) const;

  /// @}

  /// @name Data Write Functions
  /// @{

  public: void setShared(const Char *qualifier, IdentifiableObject *parent,
                         const SharedPtr<IdentifiableObject> &val) const;

  public: Bool trySetShared(const Char *qualifier, IdentifiableObject *parent,
                            const SharedPtr<IdentifiableObject> &val) const;

  public: void setPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const;

  public: Bool trySetPlain(const Char *qualifier, IdentifiableObject *parent, IdentifiableObject *val) const;

  /// @}

  /// @name Data Delete Functions
  /// @{

  public: void remove(const Char *qualifier, IdentifiableObject *parent) const;

  public: Bool tryRemove(const Char *qualifier, IdentifiableObject *parent) const;

  /// @}

}; // class

} } // namespace

#endif