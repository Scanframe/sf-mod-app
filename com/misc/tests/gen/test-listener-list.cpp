#include <misc/gen/TListener.h>
#include <misc/gen/gen_utils.h>
#include <test/catch.h>

struct MyEmitter
{
		typedef sf::TListener<std::string&> listener_type;
		listener_type::emitter_type emitter;

		// Call the listeners.
		std::string callThem()
		{
			std::string s(1, '$');
			emitter.callListeners(s);
			return s;
		}
};

// Inherit from sf:ListenerList
struct MyListener : sf::ListenerList
{
		explicit MyListener(std::string ofs)
			: offset(std::move(ofs))
		{}

		auto unlinkIt()
		{
			return delete_null(link_ptr);
		}

		void linkIt(MyEmitter* emitter)
		{
			MyEmitter::listener_type::func_type lambda = [&](std::string& s) {
				s += offset;
			};
			// Hookup a lambda function.
			link_ptr = emitter->emitter.linkListener(this, lambda);
		}

		MyEmitter::listener_type* link_ptr{nullptr};
		std::string offset;
};

TEST_CASE("sf::ListenerList", "[con][generic]")
{
	using Catch::Matchers::Equals;

	SECTION("Delete Listener")
	{
		auto emitter = new MyEmitter;
		// Create 2 listeners.
		auto listener1 = new MyListener("AB");
		auto listener2 = new MyListener("CD");
		// Hook both listeners up wit an emitter.
		listener1->linkIt(emitter);
		listener2->linkIt(emitter);
		// Cleanup should return zero.
		REQUIRE(listener1->cleanupListeners() == 0);
		// Cleanup should return zero.
		REQUIRE(emitter->emitter.cleanup() == 0);
		// The string is formed by the order of listeners.
		REQUIRE(emitter->callThem() == std::string("$ABCD"));
		// Delete the class having the listeners.
		sf::delete_null(listener1);
		// Cleanup should now return 1.
		REQUIRE(emitter->emitter.cleanup() == 1);
		// Emit the event by calling the listeners again.
		// Only listener2 is adding to the string.
		REQUIRE(emitter->callThem() == std::string("$CD"));
		// Delete the rest.
		sf::delete_null(emitter);
		sf::delete_null(listener2);
	}

	SECTION("Delete Emitter")
	{
		auto emitter = new MyEmitter;
		auto listener = new MyListener("ABC");
		// 2x hooking up the same.
		listener->linkIt(emitter);
		listener->linkIt(emitter);
		//
		REQUIRE(listener->cleanupListeners() == 0);
		// Emit even when no handlers are attached.
		REQUIRE(emitter->callThem() == std::string("$ABCABC"));
		// Delete the emitter class.
		sf::delete_null(emitter);
		//
		REQUIRE(listener->cleanupListeners() == 2);
		// Recreate the emitter class.
		emitter = new MyEmitter;
		// Hookup the emitter up.
		listener->linkIt(emitter);
		//
		sf::delete_null(listener);
	}

	SECTION("Flush Listeners")
	{
		auto emitter = new MyEmitter;
		auto listener1 = new MyListener("ABC");
		auto listener2 = new MyListener("DEF");
		// 2x hooking up the same.
		listener1->linkIt(emitter);
		listener2->linkIt(emitter);
		// Remove all links.
		REQUIRE(listener1->flushListeners());
		//
		REQUIRE(listener1->cleanupListeners() == 0);
		// Emit even when no handlers are attached.
		REQUIRE(emitter->callThem() == std::string("$DEF"));
		// Unlink the listener.
		listener2->unlinkIt();
		// Emit even when no handlers are attached.
		REQUIRE(emitter->callThem() == std::string("$"));
		// Delete the emitter class.
		sf::delete_null(emitter);
		sf::delete_null(listener1);
		sf::delete_null(listener2);
	}
}
