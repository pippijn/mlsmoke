namespace List
{
  static int
  length (value list)
  {
    CAMLparam1 (list);

    int length = 0;
    while (list != Val_emptylist)
      {
        list = Field (list, 1);
        length++;
      }

    CAMLreturnT (int, length);
  }


  template<typename Function>
  static void
  iter (value list, Function func)
  {
    CAMLparam1 (list);

    while (list != Val_emptylist)
      {
        func (Field (list, 0));
        list = Field (list, 1);
      }

    CAMLreturn0;
  }
}
