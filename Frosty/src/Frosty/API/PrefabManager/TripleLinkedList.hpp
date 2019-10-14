#ifndef TRIPLELINKEDLIST_HPP
#define TRIPLELINKEDLIST_HPP
#include "TripleLinkedList.hpp"

namespace Frosty
{
	template<class T>
	class Node
	{
	public:		//Variables

	private:	//Variables
		T m_Element;
		Node<T>* m_PreviousNode;
		Node<T>* m_NextNode;

		uint32_t* m_NrOfNodes = nullptr;
		//TripleLinkedList<T>* m_List

	public:		//Functions
		Node(T element, uint32_t* nrOfNodes);

		~Node();

		Node<T>* GetPreviousNode();
		Node<T>* GetNextNode();
		T GetElement();

		void RelinkForwardsWith(Node<T>* node);
		void RelinkBackwardsWith(Node<T>* node);




	private:	//Functions


	};

	template<class T>
	class TripleLinkedList
	{
	public:		//Variables

	protected:  //Variables



	private:	//Variables
		uint32_t m_NrOfNodes = 0;
		Node<T>* m_FirstNode = nullptr;
		Node<T>* m_LastNode = nullptr;

		friend class Node<T>;

	public:		//Functions
		TripleLinkedList();
		~TripleLinkedList();


		void ClearList(bool deleteElements = false);
		bool DeleteAt(uint32_t pos);
		bool DeleteAtFront();
		bool DeleteAtBack();

		Node<T>* InsertAt(T element, uint32_t pos);
		Node<T>* InsertAtFront(T element);
		Node<T>* InsertAtBack(T element);

		T GetAt(uint32_t pos);
		T GetFirst();
		T GetLast();

		uint32_t GetSize();


	private:	//Functions


	};



	template<class T>
	inline TripleLinkedList<T>::TripleLinkedList()
	{
		m_NrOfNodes = 0;
		m_FirstNode = nullptr;
		m_LastNode = nullptr;
	}


	template<class T>
	inline TripleLinkedList<T>::~TripleLinkedList()
	{
		ClearList(true);
	}



	template<class T>
	inline void TripleLinkedList<T>::ClearList(bool deleteElements)
	{
		Node<T>* traveler = m_FirstNode;
		Node<T>* temp = nullptr;

		


		for (uint32_t i = 0; i < m_NrOfNodes; i++)
		{
			temp = traveler->GetNextNode();
			if (deleteElements == true)
			{
				delete traveler->GetElement(); 
			}
			else
			{ 
				delete traveler; //When deleting the element, the node gets destroyed as well. 
			}
			
			traveler = temp;
		}
		m_NrOfNodes = 0;
		m_FirstNode = nullptr;
		m_LastNode = nullptr;
	}

	template<class T>
	inline bool TripleLinkedList<T>::DeleteAt(uint32_t pos)
	{
		if (pos < m_NrOfNodes && pos >= 0)
		{
			if (pos == 0)
			{
				return DeleteAtFront();
			}
			else if (pos == m_NrOfNodes - 1)
			{
				return DeleteAtBack();
			}
			else
			{
				Node<T>* traveler = m_FirstNode->GetNextNode();
				for (uint32_t i = 1; i < pos; i++)
				{
					traveler = traveler->GetNextNode();
				}

				Node<T>* next = traveler->GetNextNode();
				Node<T>* Previous = traveler->GetPreviousNode();

				delete traveler;

				Previous->RelinkForwardsWith(next);
				next->RelinkBackwardsWith(Previous);

				m_NrOfNodes--;
				return true;
			}

		}

		return false;
	}

	template<class T>
	inline bool TripleLinkedList<T>::DeleteAtFront()
	{
		if (m_FirstNode != nullptr)
		{
			m_NrOfNodes--;
			Node<T>* temp = m_FirstNode->GetNextNode();
			delete m_FirstNode;
			m_FirstNode = temp;

			if (m_NrOfNodes == 1)
			{
				m_LastNode = m_FirstNode;
			}
			return true;
		}
		return false;
	}

	template<class T>
	inline bool TripleLinkedList<T>::DeleteAtBack()
	{
		if (m_LastNode != nullptr)
		{
			m_NrOfNodes--;
			Node<T>* temp = m_LastNode->GetPreviousNode();
			delete m_LastNode;
			m_LastNode = temp;
			m_LastNode->RelinkForwardsWith(nullptr);

			if (m_NrOfNodes == 1)
			{
				m_FirstNode = m_LastNode;
			}
			return true;
		}


		return false;
	}

	template<class T>
	inline Node<T>* TripleLinkedList<T>::InsertAt(T element, uint32_t pos)
	{

		if (pos <= 0)
		{
			return InsertAtFront(element);
		}
		else if (pos >= m_NrOfNodes)
		{
			return InsertAtBack(element);
		}
		else
		{
			Node<T>* traveler = m_FirstNode->GetNextNode();
			for (uint32_t i = 1; i < pos; i++)
			{
				traveler = traveler->GetNextNode();
			}

			Node<T>* temp = FY_NEW Node<T>(element, &m_NrOfNodes);

			temp->RelinkBackwardsWith(traveler);
			temp->RelinkBackwardsWith(traveler->GetNextNode());

			traveler->GetNextNode()->RelinkBackwardsWith(temp);
			traveler->RelinkForwardsWith(temp);

			return temp;
		}
	}

	template<class T>
	inline Node<T>* TripleLinkedList<T>::InsertAtFront(T element)
	{
		m_NrOfNodes++;
		Node<T>* temp = FY_NEW Node<T>(element, &m_NrOfNodes);
		temp->RelinkBackwardsWith(nullptr);
		temp->RelinkForwardsWith(m_FirstNode);
		m_FirstNode = temp;

		if (m_NrOfNodes == 1)
		{
			m_LastNode = m_FirstNode;
		}

		return m_FirstNode;
	}

	template<class T>
	inline Node<T>* TripleLinkedList<T>::InsertAtBack(T element)
	{
		m_NrOfNodes++;
		Node<T>* temp = FY_NEW Node<T>(element, &m_NrOfNodes);
		temp->RelinkBackwardsWith(m_LastNode);
		temp->RelinkForwardsWith(nullptr);
		if (m_LastNode != nullptr)
		{
			m_LastNode->RelinkForwardsWith(temp);
		}
		m_LastNode = temp;

		if (m_NrOfNodes == 1)
		{
			m_FirstNode = m_LastNode;
		}

		return m_LastNode;
	}

	template<class T>
	inline T TripleLinkedList<T>::GetAt(uint32_t pos)
	{
		int test = m_NrOfNodes;

		if (pos <= 0)
		{
			return GetFirst();
		}
		else if (pos >= m_NrOfNodes)
		{
			return GetLast();
		}
		else
		{
			Node<T>* traveler = m_FirstNode->GetNextNode();
			for (uint32_t i = 1; i < pos; i++)
			{
				traveler = traveler->GetNextNode();
			}

			return traveler->GetElement();
		}
	}

	template<class T>
	inline T TripleLinkedList<T>::GetFirst()
	{
		return m_FirstNode->GetElement();
	}

	template<class T>
	inline T TripleLinkedList<T>::GetLast()
	{
		return m_LastNode->GetElement();
	}

	template<class T>
	inline uint32_t TripleLinkedList<T>::GetSize()
	{
		return m_NrOfNodes;
	}








	template<class T>
	inline Node<T>::Node(T element, uint32_t* nrOfNodes)
	{
		//m_List = list;
		m_NrOfNodes = nrOfNodes;
		m_Element = element;
		m_NrOfNodes = nrOfNodes;
		m_NextNode = nullptr;
		m_PreviousNode = nullptr;

	}

	template<class T>
	inline Node<T>::~Node()
	{
		*m_NrOfNodes = *m_NrOfNodes - 1;

		if (m_PreviousNode != nullptr)
		{
			m_PreviousNode->RelinkForwardsWith(m_NextNode);

		}

		if (m_NextNode != nullptr)
		{
			m_NextNode->RelinkBackwardsWith(m_PreviousNode);
		}


	
		
	}

	template<class T>
	inline Node<T>* Node<T>::GetPreviousNode()
	{
		return m_PreviousNode;
	}

	template<class T>
	inline Node<T>* Node<T>::GetNextNode()
	{
		return m_NextNode;
	}

	template<class T>
	inline T Node<T>::GetElement()
	{
		return m_Element;
	}

	template<class T>
	inline void Node<T>::RelinkForwardsWith(Node<T>* node)
	{
		m_NextNode = node;
	}

	template<class T>
	inline void Node<T>::RelinkBackwardsWith(Node<T>* node)
	{
		m_PreviousNode = node;
	}




}


#endif // !TRIPLELINKEDLIST_HPP

