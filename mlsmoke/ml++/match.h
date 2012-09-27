template<typename Enum, Enum Tag>
struct value_type
{
  typedef value type;
};

template<typename Enum, Enum... Tags>
struct tag_list
{
  typedef void tail;
};

template<typename Enum, Enum Tag0, Enum... Tags>
struct tag_list<Enum, Tag0, Tags...>
{
  static Enum const head = Tag0;
  typedef tag_list<Enum, Tags...> tail;
};

template<typename Enum, Enum Tag, Enum... Tags>
struct variant
{
  static Enum const tag = Tag;

  template<typename R, int Index, typename TagList>
  struct caller
  {
    template<typename Lambda, typename... Args>
    static R call (Lambda func, value obj, Args ...args)
    {
      return caller<R, Index - 1, typename TagList::tail>::call (func, obj, Field (obj, Index - 1), args...);
    }
  };

  template<typename R>
  struct caller<R, 0, void>
  {
    template<typename Lambda, typename... Args>
    static R call (Lambda func, value obj, Args ...args)
    {
      return func (args...);
    }
  };

  template<typename R, typename Lambda>
  static R call (Lambda func, value obj)
  {
    return caller<R, sizeof... (Tags), typename tag_list<Enum, Tags...>::tail>::call (func, obj);
  }
};


template<typename R = void>
struct match
{
  value const obj;
  char const *const caller;

  match (value obj, char const *caller)
    : obj (obj)
    , caller (caller)
  {
  }

  R with ()
  {
    failwith (caller);
    return R ();
  }

  template<typename Case, typename Lambda, typename... Cases>
  R with (Case first, Lambda func, Cases ...cases)
  {
    int tag = Is_long (obj)
      ? -1 - Int_val (obj)
      : Tag_val (obj);
    if (tag == Case::tag)
      return Case::template call<R> (func, obj);
    return with (cases...);
  }
};
