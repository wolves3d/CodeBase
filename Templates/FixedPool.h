#ifndef __FixedPool_h_included__
#define __FixedPool_h_included__


struct PoolItem
{
	int nPoolIndex;
};


template <class ItemType> class FixedPoolManager
{
	std::stack <ItemType *>	m_availableObjects;
	std::vector <ItemType *> m_usedObjects;

public:

	bool Init(ItemType * pArray, unsigned int nSize)
	{
		Release();

		m_usedObjects.reserve(nSize);

		while (nSize--)
		{
			ItemType * pItem = pArray + nSize;
			pItem->nPoolIndex = nSize;

			m_availableObjects.push(pItem);
		}
		
		return true;
	}

	void Release()
	{
		while ( !m_availableObjects.empty() )
			m_availableObjects.pop();

		m_usedObjects.clear();
	}

	/**
	Порядок итемов непредсказуем! (может меняться при каждом вызове RemoveObject)
	*/
	std::vector <ItemType *> & GetObjects()
	{
		return m_usedObjects;
	}

	ItemType * CreateObject()
	{
		if (0 == m_availableObjects.size())
		{
			assert(false);
			return NULL;
		}

		ItemType * pResult = m_availableObjects.top();
		m_availableObjects.pop();

		m_usedObjects.push_back(pResult);
		pResult->nPoolIndex = (m_usedObjects.size() - 1);
		
		return pResult;
	}

	void RemoveObject(ItemType * pItem)
	{
		assert(-1 != pItem->nPoolIndex); // Invalid item!

		if (-1 == pItem->nPoolIndex)
			return;

		int nCount = m_usedObjects.size();

		if (0 == nCount) // Early exit - no elements in pool
		{
			assert(false); // "DNA warning! Trying to remove element from empty pool");
			return;
		}

		int nTail = ( nCount - 1 );

		if (nTail > 0)
		{
			// Сначала удаляем хвостовой элемент
			ItemType * pTailItem = m_usedObjects[nTail];
			m_usedObjects.erase( m_usedObjects.begin() + nTail );

			// При неоходимости, переносим его на место удаленного
			if (nTail != pItem->nPoolIndex)
			{
				// Переносим хвостовой элемент на место удаленного
				m_usedObjects[ pItem->nPoolIndex ] = pTailItem;
				pTailItem->nPoolIndex = pItem->nPoolIndex;
			}
		}
		else
		{
			// В пуле был всего один элемент был и тот удалили
			m_usedObjects.erase( m_usedObjects.begin() );
		}

		pItem->nPoolIndex = -1;
		m_availableObjects.push(pItem);
	}
};

#endif // #ifndef __FixedPool_h_included__