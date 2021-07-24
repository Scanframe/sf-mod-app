# Listener & Emitter Class Example {#sf-TListener}

## Emitter Class

Create an emitting class having a `listener_type` from template #sf::TListener.<br>
Instantiate the type 

```c++
class MyEmitter
{
  public:
    typedef sf::TListener<std::string&> listener_type;
    listener_type::emitter_type emitter;

    std::string callThem()
    {
        std::string s(1, '$');
        emitter.callListeners(s);
        return s;
    }
};
```

## Listener Class

Derive the class from #sf::ListenerList to link/register any type of listener.

```c++
class MyListener :sf::ListenerList
{
  public:
    explicit MyListener(std::string ofs)
        :offset(std::move(ofs)) {}

    auto unlinkIt()
    {
        return delete_null(link_ptr);
    }

    void linkIt(MyEmitter* emitter)
    {
        MyEmitter::listener_type::func_type lambda = [&](std::string& s)
        {
            s += offset;
        };

        link_ptr = emitter->emitter.linkListener(this, lambda);
    }
  
  private:  
    MyEmitter::listener_type* link_ptr{nullptr};
    std::string offset;
};
```
