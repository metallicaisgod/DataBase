namespace CDICommon
{
#pragma once

class NodeTL {
protected:
	NodeTL *_next;	// связь к последующему узлу
	NodeTL *_prev;	// связь к предшествующему узлу
public:
	NodeTL (void)
	{
		_next = _prev = this;	
	}
	virtual ~NodeTL (void){}
	NodeTL* next(void){return _next;}
	NodeTL* prev(void){return _prev;}
	NodeTL* insert(NodeTL* b)
	{
		NodeTL *c = _next;
		b->_next = c;
		b->_prev = this ;
		_next = b;
		c->_prev = b;
		return b;
	}

	NodeTL* remove(void)
	{
		_prev->_next = _next;
		_next->_prev = _prev;
		_next = _prev = this;
		return  this;
	}
	void revert()
	{
		NodeTL* tmp = _prev;
		_prev = _next;
		_next = tmp;
	}
};

template<class T> class ListNodeTL : public NodeTL
{
public :
  T _val;
  ListNodeTL (T val): _val(val){}
};

template<class T> class ListTL {
protected:
	ListNodeTL<T>* _curr;
	int _length;
public :
	ListTL (void)
	{
		_curr = 0;
		_length = 0;
	}
	~ListTL (void)
	{
		while (length() > 0)
		{
			remove ();
		}
	}
	void revert()
	{
		int count = length();
		NodeTL* cur =  curr();
		while (count--)
		{
			cur->revert();
			cur = cur->next();
		}
	}
	ListNodeTL<T>* insert (T& in_val)
	{
		if(_curr == 0)
		{
			_curr = new ListNodeTL<T>(in_val);
		}
		else
			_curr->insert(new ListNodeTL<T>(in_val));
		_length++;
		return (ListNodeTL<T>*)_curr->next();
	}

	ListNodeTL<T>* insert_after (T& in_val)
	{
		if(_curr == 0)
		{
			_curr = new ListNodeTL<T>(in_val);
		}
		else
			_curr->insert(new ListNodeTL<T>(in_val));
		_length++;
		return (ListNodeTL<T>*)_curr->next();
	}

	ListNodeTL<T>* insert_before (T& in_val)
	{
		if(_curr == 0)
		{
			_curr = new ListNodeTL<T>(in_val);
		}
		else
			_curr->prev()->insert(new ListNodeTL<T>(in_val));
		_length++;
		return (ListNodeTL<T>*)_curr->next();
	}

	ListNodeTL<T>* remove (void)
	{
		if (_curr == 0)
		{
			return 0;
		}
		_curr = (ListNodeTL<T>*) _curr->prev();
		delete (ListNodeTL<T>*) _curr->next()->remove();
		_length--;
		if (_length == 0)_curr = 0;
		return (ListNodeTL<T>*)_curr;
	}

	T val (void)
	{
		return _curr->_val;
	}

	ListNodeTL<T>* curr (void)
	{
		return _curr;
	}

	void	curr (ListNodeTL<T>* in_node)
	{
		_curr = in_node;
	}

	T	next (void)
	{
		T val = _curr->_val;
		_curr = (ListNodeTL<T>*) _curr->next();
		return val;
	}

	T	prev(void)
	{
		T val = _curr->_val;
		_curr = (ListNodeTL<T>*) _curr->prev();
		return val;
	}

	int length (void)
	{
		return _length;
	}

	// stuff to support STL shit
	ListNodeTL<T>* push_back (T in_val)
	{
		if(_curr == 0)
		{
			_curr = new ListNodeTL<T>(in_val);
		}
		else
			_curr->insert(new ListNodeTL<T>(in_val));
		_length++;
		return (ListNodeTL<T>*)_curr->next();
	}
	int size (void)
	{
		return _length;
	}

};


}