Yet Another [RMI](http://en.wikipedia.org/wiki/Java_remote_method_invocation)
=========

RMI constructor using C++ preprocessor

### Для чего "оно" нужно, и с чем "его" едят?

Представьте, что вам нужно реализовать взаимодействие клиент-сервер, по принципу [RMI](http://en.wikipedia.org/wiki/Java_remote_method_invocation). Что для этого нужно, вы наверняка знаете:

1. на вызывающей стороне описать функцию с сигнатурой, соответствующей вызываемой процедуре
2. сериализовать аргументы вызова
3. на вызываемой стороне описать вызываемую процедуру с сигнатурой, соответствующей вызывающей стороне
4. десериализовать пришедшие данные
5. подставить их в вызов соответствующей процедуры
6. произвести вызов соответствующей процедуры

Все мы это делали множество раз, и во всем этом нет ничего сложного, с одной лишь оговоркой - когда кол-во процедур и их аргументов исчисляется единицами ;)

#### *И все же, для чего "оно" нужно?*
Терпение, друг мой... Сейчас все будет.

Описываем имена объектов-обработчиков, и имена процедур в синтаксисе препроцессора:
```cpp
YARMI_CONSTRUCT(
    client_invoker,
    ((pong,
        ((std::string))
    )),
    server_invoker,
    ((ping,
        ((std::string))
    ))
)
```
Всю остальную работу, YARMI(`YARMI_CONSTRUCT()`) сделает за нас!

#### *Что именно "оно" сделает?*
Ну вот же:
```cpp
namespace yarmi {

template<typename Impl, typename IO = Impl>
struct client_invoker {
	client_invoker(Impl *impl, IO *io)
		:impl(impl)
		,io(io)
	{}
	
	void ping(const std::string &arg0) {
		yas::binary_mem_oarchive oa(yas::no_header);
		oa & static_cast<std::uint8_t>(0)
			& static_cast<std::uint8_t>(0)
			& arg0;
		yas::binary_mem_oarchive pa;
		pa & oa.get_intrusive_buffer();
		io->send(pa.get_shared_buffer());
	}
	
	void yarmi_error(const std::uint8_t &arg0, const std::uint8_t &arg1, const std::string &arg2) {
		yas::binary_mem_oarchive oa(yas::no_header);
		oa & static_cast<std::uint8_t>(1)
			& static_cast<std::uint8_t>(0)
			& arg0
			& arg1
			& arg2;
		yas::binary_mem_oarchive pa;
		pa & oa.get_intrusive_buffer();
		io->send(pa.get_shared_buffer());
	}
	
	void invoke(const char *ptr, std::size_t size) {
		std::uint8_t call_id, call_version;
		static const char* names[] = {
			 "pong"
			,"yarmi_error"
		};
		static const std::uint8_t versions[] = {
			 0
			,0
		};
		
		try {
			yas::binary_mem_iarchive ia(ptr, size, yas::no_header);
			ia & call_id
				& call_version;
			if ( call_id < 0 || call_id > 1 ) {
				char errstr[1024] = {0};
				std::snprintf(
					 errstr
					,sizeof(errstr)
					,"%s::%s(): bad call_id %d"
					,"client_invoker"
					,__FUNCTION__
					,static_cast<int>(call_id)
				);
				throw std::runtime_error(errstr);
			}
			if ( call_version > versions[call_id] ) {
				char errstr[1024] = {0};
				std::snprintf(
					 errstr
					,sizeof(errstr)
					,"%s::%s(): bad call_version %d for call_id %d(%s::%s())"
					,"client_invoker"
					,__FUNCTION__
					,static_cast<int>(call_version)
					,static_cast<int>(call_id)
					,"client_invoker"
					,names[call_id]
				);
				throw std::runtime_error(errstr);
			}
			
			switch ( call_id ) {
				case 0: {
					switch ( call_version ) {
						case 0: {
							std::string arg0;
							ia & arg0;
							impl->on_pong(arg0);
						};
						break;
					}
				};
				break;
				case 1: {
					switch ( call_version ) {
						case 0: {
							std::uint8_t arg0;
							std::uint8_t arg1;
							std::string arg2;
							ia & arg0
								& arg1
								& arg2;
							impl->on_yarmi_error(arg0, arg1, arg2);
						};
						break;
					}
				};
				break;
			}
		} catch (const std::exception &ex) {
			char errstr[1024] = {0};
			std::snprintf(
				 errstr
				,sizeof(errstr)
				,"std::exception is thrown when %s::%s() is called: '%s'"
				,"client_invoker"
				,names[call_id]
				,ex.what()
			);
			yarmi_error(call_id, call_version, errstr);
		} catch (...) {
			char errstr[1024] = {0};
			std::snprintf(
				 errstr
				,sizeof(errstr)
				,"unknown exception is thrown when %s::%s() is called"
				,"client_invoker"
				,names[call_id]
			);
			yarmi_error(call_id, call_version, errstr);
		}
	}

private:
	Impl *impl;
	IO *io;
}; // struct client_invoker

template<typename Impl, typename IO = Impl>
struct server_invoker {
	server_invoker(Impl *impl, IO *io)
		:impl(impl)
		,io(io)
	{}
	
	void pong(const std::string &arg0) {
		yas::binary_mem_oarchive oa(yas::no_header);
		oa & static_cast<std::uint8_t>(0)
			& static_cast<std::uint8_t>(0)
			& arg0;
		yas::binary_mem_oarchive pa;
		pa & oa.get_intrusive_buffer();
		io->send(pa.get_shared_buffer());
	}
	
	void yarmi_error(const std::uint8_t &arg0, const std::uint8_t &arg1, const std::string &arg2) {
		yas::binary_mem_oarchive oa(yas::no_header);
		oa & static_cast<std::uint8_t>(1)
			& static_cast<std::uint8_t>(0)
			& arg0
			& arg1
			& arg2;
		yas::binary_mem_oarchive pa;
		pa & oa.get_intrusive_buffer();
		io->send(pa.get_shared_buffer());
	}
	
	void invoke(const char *ptr, std::size_t size) {
		std::uint8_t call_id, call_version;
		static const char* names[] = {
			 "ping"
			,"yarmi_error"
		};
		static const std::uint8_t versions[] = {
			 0
			,0
		};
		
		try {
			yas::binary_mem_iarchive ia(ptr, size, yas::no_header);
			ia & call_id
				& call_version;
			if ( call_id < 0 || call_id > 1 ) {
				char errstr[1024] = {0};
				std::snprintf(
					 errstr
					,sizeof(errstr)
					,"%s::%s(): bad call_id %d"
					,"server_invoker"
					,__FUNCTION__
					,static_cast<int>(call_id)
				);
				throw std::runtime_error(errstr);
			}
			if ( call_version > versions[call_id] ) {
				char errstr[1024] = {0};
				std::snprintf(
					 errstr
					,sizeof(errstr)
					,"%s::%s(): bad call_version %d for call_id %d(%s::%s())"
					,"server_invoker"
					,__FUNCTION__
					,static_cast<int>(call_version)
					,static_cast<int>(call_id)
					,"server_invoker"
					,names[call_id]
				);
				throw std::runtime_error(errstr);
			}
			
			switch ( call_id ) {
				case 0: {
					switch ( call_version ) {
						case 0: {
							std::string arg0;
							ia & arg0;
							impl->on_ping(arg0);
						};
						break;
					}
				};
				break;
				case 1: {
					switch ( call_version ) {
						case 0: {
							std::uint8_t arg0;
							std::uint8_t arg1;
							std::string arg2;
							ia & arg0
								& arg1
								& arg2;
							impl->on_yarmi_error(arg0, arg1, arg2);
						};
						break;
					}
				};
				break;
			}
		} catch (const std::exception &ex) {
			char errstr[1024] = {0};
			std::snprintf(
				 errstr
				,sizeof(errstr)
				,"std::exception is thrown when %s::%s() is called: '%s'"
				,"server_invoker"
				,names[call_id]
				,ex.what()
			);
			yarmi_error(call_id, call_version, errstr);
		} catch (...) {
			char errstr[1024] = {0};
			std::snprintf(
				 errstr
				,sizeof(errstr)
				,"unknown exception is thrown when %s::%s() is called"
				,"server_invoker"
				,names[call_id]
			);
			yarmi_error(call_id, call_version, errstr);
		}
	}

private:
	Impl *impl;
	IO *io;
}; // struct server_invoker

} // ns yarmi

```

1. Метод `client_invoker::ping()` соответствует описанному нами в макросе `YARMI_CONSTRUCT()`
2. Метод `client_invoker::yarmi_error()` - это системный метод, используется для сообщения противоположной стороне о том, что при попытке произвести вызов произошла ошибка. Посмотрите внимательно, в `client_invoker::invoke()`, десериализация и вызов обернуты в `try{}catch()` блок, а в `catch()` блоках производится вызов `yarmi_error()`. Таким образом, если при десериализации или вызове процедуры возникнет исключение - оно будет успешно перехвачено `catch()` блоком, и информация об исключении будет отправлена вызывающей стороне. То же самое будет происходить и в противоположном направлении. Т.е. если сервер вызвал у клиента процедуру, в ходе вызова которой возникло исключение - клиент отправит серверу информацию об ошибке, так же дополнительно сообщив ID и версию вызова, в котором возникло исключение. Но использовать `yarmi_error()` вы можете и сами, ничто этого не запрещает. Пример: `yarmi_error(call_id, version_id, "message");`
3. В методе `server_invoker::invoke()`, в `switch`е, производится десериализация аргументов, подставновка их в вызов, и, собственно, вызов `on_ping()`. Помните, что мы со стороны клиента производим вызов `ping()`? Так вот, реализация этого метода на стороне сервера зовется `on_ping()`. Т.е. к имени каждой вызываемой процедуры на стороне ее реализации, добавляется префикс `on_`.
4. В методе `client_invoker::invoke()`, в `switch`е, происходит тоже самое с тем лишь отличием, что проиходит вызов метода `on_pong()`. Сервер пошлет ответ клиенту, вызвав этот метод. Т.е. порядок вызовов такой: `client_invoker::ping()` -> `server_invoker::on_ping()` -> `server_invoker::pong()` -> `client_invoker::on_pong()`.
5. Вам не нужно более заботиться ни о соответствии сигнатур, ни о идентификаторах, ни о сериализаторах/десериализаторах.

В добавок ко всему этому, одним немаловажным плюсом является и то, что сейчас невероятно сложно допустить ошибку. И, даже если мы ее допустим - узнаем мы об этом во время собрки проекта.

Как вы могли заметить, классы `client_invoker` и `server_invoker` принимают два параметра. Первый их низ - класс в котором реализованы вызываемые процедуры, второй - класс в котором реализован метод `send(yas::shared_buffer buf)`. (в качестве сериализации/десериализации используется другой мой проект - [YAS](https://github.com/niXman/yas))

#### *Как предполагается "это" использовать в конечном счете?*
Опять же - все просто!

```cpp
// client side
struct client: yarmi::client_base, yarmi::client_invoker<client> {
    client(boost::asio::io_service &ios)
        :yarmi::client_base(ios)
        ,yarmi::client_invoker<client>(this, this)
    {
        // запрос к серверу
        ping("Hello, server!");
    }
    
    // на ответ сервера вызывается этот метод
    void on_pong(const std::string &msg) {
        std::cout << "server ask:\"" << msg << "\"" << std::endl;
        // из этого метода мы снова зовем сервер
        ping("Hello, server, again!");
    }
};

// server side
struct session: yarmi::session_base, yarmi::server_invoker<session> {
    session(boost::asio::io_service &ios)
        :yarmi::session_base(ios)
        ,yarmi::server_invoker<session>(this, this)
    {}
    
    // вызывается при запросе клиента
    void on_ping(const std::string &msg) {
        // отвечаем
        if ( msg.find("again") == std::string::npos )
            pong("Hello, client!");
        else
            pong("Hello, client, again!");
    }
};
```

Ну как ты, Шарапов, впечатлен? ;)

Еще одна, немаловажная для интернет-сервисов плюшка - версионность процедур. Это позволит нам создавать новые версии API нашего сервиса, и при этом, не требуя обновления наших клиентов.
т.е., к примеру, мы может сделать такое:
```cpp
YARMI_CONSTRUCT(
    client_invoker,
    ((pong,
        ((std::string))
        ((std::string, std::string)) // это вторая версия процедуры 'pong()'
    )),
    server_invoker,
    ((ping,
        ((std::string))
        ((std::string, std::string)) // это вторая версия процедуры 'ping()'
    ))
)
```
Таким образом, клинет может может вызывать как `ping("message")` так и `ping("message", "user name")`. И, соответственно, сервер может слать два типа ответа. И снова повторюсь: ничего руками делать не нужно! соответствие `ID`ов процедур и их версий - это забота yarmi!

### Более сложный пример
```cpp
YARMI_CONSTRUCT(
	client_invoker, // name of the client invoker
	((registration,
		((std::string, std::string)) // message : registration key
	))
	((activation,
		((std::string)) // message
	))
	((login,
		((std::string)) // message
	))
	((logout,
		((std::string)) // message
	))
	((users_online,
		((std::vector<std::string>)) // without args
	))
	,
	server_invoker, // name of the server invoker
	((registration,
		((std::string)) // username
	))
	((activation,
		((std::string, std::string, std::string)) // registration key : username : password
	))
	((login,
		((std::string, std::string)) // username : password
	))
	((logout,
		(()) // without args
	))
	((users_online,
		(()) // without args
	))
);
```
Результат [тут](https://github.com/niXman/yarmi/blob/master/examples/chat/protocol-preprocessed.hpp).

Как вы могли заметить, имена процедур на стороне сервера и клиента пересекаются. Да, в этом нет ничего необычного ;)



TODO
=========

1. Генерация нескольких интерфейсов
2. Описать спецификацию (хоть она и проста больше некуда)
3. Возможность использовать собственный алокатор
4. Преаллоцированные хендлеры по умолчанию
