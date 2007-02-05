
#pragma once

#define BEFORE_START_POSITION ((POSITION)-1L)


struct CPlex     // warning variable length structure
{
	CPlex* pNext;
#ifndef _WIN64
#if (_AFX_PACKING >= 8)
	DWORD dwReserved[1];    // align on 8 byte boundary
#endif
#endif
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CPlex* PASCAL Create(CPlex*& head, UINT_PTR nMax, UINT_PTR cbElement);
	// like 'calloc' but no zero fill
	// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};

class CMapPtrToPtr 
{
	
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;

		void* key;
		void* value;
	};

	struct __POSITION
	{
	};
	typedef __POSITION* POSITION;

public:

	inline INT_PTR CMapPtrToPtr::GetCount() const
	{ return m_nCount; }
	inline INT_PTR CMapPtrToPtr::GetSize() const
	{ return m_nCount; }
	inline BOOL CMapPtrToPtr::IsEmpty() const
	{ return m_nCount == 0; }
	inline void CMapPtrToPtr::SetAt(void* key, void* newValue)
	{ (*this)[key] = newValue; }
	inline POSITION CMapPtrToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
	inline UINT CMapPtrToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }

	// Construction
	/* explicit */ CMapPtrToPtr(INT_PTR nBlockSize = 10);

	// Attributes
	// number of elements
	//INT_PTR GetCount() const;
	//INT_PTR GetSize() const;
	//BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(void* key, void*& rValue) const;

	// Operations
	// Lookup and add if not there
	void*& operator[](void* key);

	// add a new (key, value) pair
	//void SetAt(void* key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(void* key);
	void RemoveAll();

	// iterating all (key, value) pairs
	//POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, void*& rKey, void*& rValue) const;

	// advanced features for derived classes
	//UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(void* key) const;

	// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	INT_PTR m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	INT_PTR m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(void*, UINT&, UINT&) const;

public:
	~CMapPtrToPtr();
#ifdef _DEBUG
//	void Dump(CDumpContext&) const;
//	void AssertValid() const;
#endif

	void* GetValueAt(void* key) const;


protected:
	// local typedefs for CTypedPtrMap class template
	typedef void* BASE_KEY;
	typedef void* BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};

