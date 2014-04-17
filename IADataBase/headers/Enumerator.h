#if !defined(__DB_ENUMERATOR_H__)
#define __DB_ENUMERATOR_H__	1
/********************************************************************
	created:	2005/05/08
	created:	8:5:2005   23:19
	filename: 	Enumerator.h
	file base:	Enumerator
	file ext:	h
	author:		Migel
	purpose:	Template definition of the enumerator class
	That can be enumerated over collection
*********************************************************************/

namespace db
{
	//////////////////////////////////////////////////////////////////////////
	// Functor - empty filter
	// all records in collection will be enumerated
	template <typename T>
	struct no_filter : public std::unary_function<T, bool>
	{
		bool operator()(const T& )
		{return true;}
	};
	
	template<typename iterator, typename predicat=no_filter<typename iterator::value_type> >
	class enumerator
	{
	private:
		iterator	m_begin;
		iterator	m_end;
		iterator	m_current;
		bool		m_atStart;
		predicat	m_predicate;
	public:
		enumerator(iterator begin, iterator end, predicat pred) :
		  m_begin(begin), m_current(begin), m_end(end), m_predicate(pred), m_atStart(true)
		{
		}
		enumerator(const enumerator<iterator, predicat>& src):
			m_begin(src.m_begin), m_current(src.m_begin), m_end(src.m_end), 
			m_predicate(src.m_predicate), m_atStart(true)
		{
		}

		virtual ~enumerator(){;}
		
		typename iterator::value_type& GetCurrent() const
		{
			return *m_current;
		}

		// Move current recored pointer ro the next record
		bool MoveNext()
		{
			if (m_atStart)
				m_atStart = false;
			else
			increment();
			if (m_current == m_end)
				return false;
			return true;
		}

		// Move current recored pointer to the prev record
		bool MovePrev()
		{
			if (m_begin == m_current)
				return false;

			decrement();
			return true;
		}

	private:

		void decrement()
		{
			while(!this->m_predicate(*--(this->m_current) && m_current != m_begin))
			{};
		}

		void increment()
		{
			if (m_current == m_end)
				return;
			++(this->m_current);
			satisfy_predicate();
		}

		void satisfy_predicate()
		{
			while (this->m_current != this->m_end && !this->m_predicate(*m_current))
			{
				if(m_current!=m_end)
					++(this->m_current);
			}
		}
	};

	
	//template<typename collection, typename predicat=no_filter<typename collection::iterator::value_type> >
	//class enumerator_ref : public enumerator<typename collection::iterator, predicat>
	//{
	//public:
	//	typedef boost::shared_ptr<collection> t_CollPtr;

	//	
	//	enumerator_ref(t_CollPtr collection, predicat pred) :
	//	enumerator<collection::iterator, predicat>(collection->begin(), collection->end(), pred),
	//		m_collection(collection)
	//	  {
	//	  }

	//private:
	//	t_CollPtr m_collection;
	//};

	template<typename collection, typename predicat=no_filter<typename collection::iterator::value_type> >
	class enumerator_ref2ptr : public enumerator<typename collection::iterator, predicat>
	{
	public:
		enumerator_ref2ptr(collection *collection, predicat pred) :
			enumerator<collection::iterator, predicat>(collection->begin(), collection->end(), pred),
			m_collection(collection)
		  {
		  }
		virtual ~enumerator_ref2ptr()
		{
			delete m_collection;
		}

	private:
		collection *m_collection;
		
	};


}
#endif // __DB_ENUMERATOR_H__