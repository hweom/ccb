#pragma once

#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <cxxabi.h>

namespace ccb { namespace mock
{
    namespace mock_details
    {
        static constexpr size_t MAX_VIRTUAL_FUNCTIONS = 30;

        template<typename A>
        union MemberPtrCastUnion
        {
            A a;
            void *ptr;
        };

        struct Utils
        {
            template<typename Func>
            static void* CastMemberFunctionPtr(Func ptr)
            {
                MemberPtrCastUnion<Func> u;
                u.a = ptr;
                return u.ptr;
            }

            static std::string TypeToString(const std::type_info& type)
            {
                int status;
                auto realname = abi::__cxa_demangle(type.name(), 0, 0, &status);
                std::string result(realname);
                free(realname);

                return result;
            }
        };

        class VirtualFunctionIndexer
        {
        public:
            virtual size_t F00() { return 0; }
            virtual size_t F01() { return 1; }
            virtual size_t F02() { return 2; }
            virtual size_t F03() { return 3; }
            virtual size_t F04() { return 4; }
            virtual size_t F05() { return 5; }
            virtual size_t F06() { return 6; }
            virtual size_t F07() { return 7; }
            virtual size_t F08() { return 8; }
            virtual size_t F09() { return 9; }
            virtual size_t F10() { return 10; }
            virtual size_t F11() { return 11; }
            virtual size_t F12() { return 12; }
            virtual size_t F13() { return 13; }
            virtual size_t F14() { return 14; }
            virtual size_t F15() { return 15; }
            virtual size_t F16() { return 16; }
            virtual size_t F17() { return 17; }
            virtual size_t F18() { return 18; }
            virtual size_t F19() { return 19; }
            virtual size_t F20() { return 20; }
            virtual size_t F21() { return 21; }
            virtual size_t F22() { return 22; }
            virtual size_t F23() { return 23; }
            virtual size_t F24() { return 24; }
            virtual size_t F25() { return 25; }
            virtual size_t F26() { return 26; }
            virtual size_t F27() { return 27; }
            virtual size_t F28() { return 28; }
            virtual size_t F29() { return 29; }
        };

        template<typename Ret, typename... Params>
        class CallAction
        {
        public:

            virtual ~CallAction() {}

            virtual Ret Invoke(Params... params) = 0;
        };

        template<typename... Params>
        class EmptyCallAction : public CallAction<void, Params...>
        {
        public:

            virtual void Invoke(Params... params) override
            {
            }
        };

        template<typename Ret, typename... Params>
        class RedirectedCallAction : public CallAction<Ret, Params...>
        {
        private:

            std::function<Ret(Params...)> redirection;

        public:

            RedirectedCallAction(const std::function<Ret(Params...)>& redirection)
                : redirection(redirection)
            {
            }

        public:

            virtual Ret Invoke(Params... params) override
            {
                return this->redirection(params...);
            }
        };

        template<typename Ret, typename... Params>
        class ReturnCallAction : public CallAction<Ret, Params...>
        {
        private:

            Ret value;

        public:

            ReturnCallAction(const Ret& value)
                : value(value)
            {
            }

        public:

            virtual Ret Invoke(Params... params) override
            {
                return this->value;
            }
        };

        template<typename Ret, typename... Params>
        class ReturnRefCallAction : public CallAction<Ret, Params...>
        {
        private:

            std::reference_wrapper<Ret> value;

        public:

            ReturnRefCallAction(const std::reference_wrapper<Ret>& value)
                : value(value)
            {
            }

        public:

            virtual Ret Invoke(Params... params) override
            {
                return this->value.get();
            }
        };

        class MockedCallBase
        {
        public:
            virtual ~MockedCallBase() { }

            /// Verify expectations.
            virtual std::pair<std::string, bool> Verify() = 0;
        };

        template<typename Ret, typename... Params>
        class TypedMockedCall : public MockedCallBase
        {
        private:

            /// Call name.
            std::string name;

            /// Call action.
            std::unique_ptr<CallAction<Ret, Params...>> callAction;

            /// Number of times this function was called.
            int timesCalled;

        protected:

            /// Number of times this function was expected to be called.
            int timesToCall;

        public:

            TypedMockedCall(const std::string& name)
                : name(name)
                , timesCalled(0)
                , timesToCall(1)
            {

            }

        public:

            /// Verify expectations.
            virtual std::pair<std::string, bool> Verify() override
            {
                std::ostringstream stream;

                if ((this->timesToCall >= 0) && (this->timesCalled != this->timesToCall))
                {
                    stream << this->name << " was called " << this->timesCalled << " out of expected " << this->timesToCall;
                    return std::make_pair(stream.str(), false);
                }

                return std::make_pair(std::string(), true);
            }

        protected:

            void SetCallAction(CallAction<Ret, Params...>* action)
            {
                this->callAction = std::unique_ptr<CallAction<Ret, Params...>>(action);
            }

            Ret Do(Params... params)
            {
                this->timesCalled++;

                if (this->callAction == nullptr)
                {
                    throw std::logic_error("Call action not set for " + this->name);
                }

                return this->callAction->Invoke(params...);
            }

            template<class T>
            friend class MockObject;
        };

        template<typename Ret, typename... Params>
        class MockedCall : public TypedMockedCall<Ret, Params...>
        {
        public:

            MockedCall(const std::string& name)
                : TypedMockedCall<Ret, Params...>(name)
            {
            }

        public:

            MockedCall<Ret, Params...>& Times(int timeToCall)
            {
                this->timesToCall = timeToCall;
                return *this;
            }

            MockedCall<Ret, Params...>& AnyTimes()
            {
                this->timesToCall = -1;
                return *this;
            }

            MockedCall<Ret, Params...>& Redirect(const std::function<Ret(Params...)>& callback)
            {
                this->SetCallAction(new RedirectedCallAction<Ret, Params...>(callback));
                return *this;
            }

            MockedCall<Ret, Params...>& ReturnRef(const std::reference_wrapper<Ret>& value)
            {
                this->SetCallAction(new ReturnRefCallAction<Ret, Params...>(value));
                return *this;
            }

            MockedCall<Ret, Params...>& Return(const Ret& value)
            {
                this->SetCallAction(new ReturnCallAction<Ret, Params...>(value));
                return *this;
            }
        };

        template<typename... Params>
        class MockedCall<void, Params...> : public TypedMockedCall<void, Params...>
        {
        public:

            MockedCall(const std::string& name)
                : TypedMockedCall<void, Params...>(name)
            {
                this->SetCallAction(new EmptyCallAction<Params...>());
            }

        public:

            MockedCall<void, Params...>& Times(int timeToCall)
            {
                this->timesToCall = timeToCall;
            }

            MockedCall<void, Params...>& AnyTimes()
            {
                this->timesToCall = -1;
            }

            MockedCall<void, Params...>& Redirect(const std::function<void(Params...)>& callback)
            {
                this->SetCallAction(new RedirectedCallAction<void, Params...>(callback));
                return *this;
            }
        };

        template<size_t I>
        class CallHandlerGenerator
        {
        public:
            template<class MT, typename Ret, typename... Params>
            static typename MT::TrivialFunc Generate(size_t index)
            {
                if (I == index)
                {
                    return MT::template GetCallHandler<I, Ret, Params...>();
                }
                else
                {
                    return CallHandlerGenerator<I+1>::template Generate<MT, Ret, Params...>(index);
                }
            }
        };

        template<>
        class CallHandlerGenerator<MAX_VIRTUAL_FUNCTIONS>
        {
        public:
            template<class MT, typename Ret, typename... Params>
            static typename MT::TrivialFunc Generate(size_t index)
            {
                throw std::logic_error("Mock: maximum number of virtual functions reached.");
            }
        };

        template<class T>
        class MockObject
        {
        public:

            typedef void (MockObject<T>::*TrivialFunc)();

        private:

            void* vtable[MAX_VIRTUAL_FUNCTIONS];

            void* oldVtable;

            std::map<size_t, MockedCallBase*> callTable;

        public:

            MockObject()
            {
                // Save old vtable and rewrite it with custom one.
                this->oldVtable = *reinterpret_cast<void**>(this);
                *reinterpret_cast<void**>(this) = this->vtable;

                for (size_t i = 0; i < MAX_VIRTUAL_FUNCTIONS; i++)
                {
                    this->vtable[i] = Utils::CastMemberFunctionPtr(&MockObject::NotImplemented);
                }
            }

            virtual ~MockObject()
            {
                // Restore old vtable;
                *reinterpret_cast<void**>(this) = this->oldVtable;

                // Verify all expectations.
                this->VerifyExpectations();
            }

        public:

            template<typename Ret, typename... Params>
            MockedCall<Ret, Params...>& Register(Ret (T::*func)(Params...))
            {
                auto index = this->GetIndex(func);

                auto mockedCall = new MockedCall<Ret, Params...>(Utils::TypeToString(typeid(func)));
                this->callTable[index] = mockedCall;

                this->vtable[index] = Utils::CastMemberFunctionPtr(CallHandlerGenerator<0>::Generate<MockObject<T>, Ret, Params...>(index));

                return *mockedCall;
            }

            void VerifyExpectations()
            {
                bool haveFailed = false;
                std::ostringstream stream;
                stream << "The following expectations were not met for " << Utils::TypeToString(typeid(T)) << ": " << std::endl;

                for (auto& pair : this->callTable)
                {
                    auto result = pair.second->Verify();
                    if (!result.second)
                    {
                        stream << result.first << std::endl;

                        haveFailed = true;
                    }
                }

                if (haveFailed)
                {
                    throw std::logic_error(stream.str());
                }
            }

        private:

            template<typename Ret, typename... Params>
            size_t GetIndex(Ret (T::*func)(Params...))
            {
                VirtualFunctionIndexer indexer;
                auto ptr = reinterpret_cast<size_t (VirtualFunctionIndexer::*)()>(func);
                return (indexer.*ptr)();
            }

            template<size_t I, typename Ret, typename... Params>
            Ret CallHandler(Params... params)
            {
                if (this->callTable.count(I) == 0)
                {
                    throw std::logic_error("Not implemented");
                }

                return static_cast<MockedCall<Ret, Params...>*>(this->callTable[I])->Do(params...);
            }

            template<size_t I, typename Ret, typename... Params>
            static TrivialFunc GetCallHandler()
            {
                return reinterpret_cast<TrivialFunc>(&MockObject<T>::CallHandler<I, Ret, Params...>);
            }

            void NotImplemented()
            {
                throw std::logic_error("Unexpected mock call");
            }

            template<size_t I>
            friend class CallHandlerGenerator;
        };
    }

    template<class T>
    class Mock
    {
    private:

        mock_details::MockObject<T> object;

    public:

        T& GetObject()
        {
            return *reinterpret_cast<T*>(&this->object);
        }

        template<typename Ret, typename... Params, typename Callback>
        mock_details::MockedCall<Ret, Params...>& ExpectCall(Ret (T::*func)(Params...), Callback cb)
        {
            return this->object.Register(func).Redirect(cb);
        }

        template<typename Ret, typename... Params, typename Callback>
        mock_details::MockedCall<Ret, Params...>& ExpectCall(Ret (T::*func)(Params...) const, Callback cb)
        {
            return this->object.Register(reinterpret_cast<Ret (T::*)(Params...)>(func)).Redirect(cb);
        }

        template<typename Ret, typename... Params>
        mock_details::MockedCall<Ret, Params...>& ExpectCall(Ret (T::*func)(Params...))
        {
            return this->object.Register(func);
        }

        template<typename Ret, typename... Params>
        mock_details::MockedCall<Ret, Params...>& ExpectCall(Ret (T::*func)(Params...) const)
        {
            return this->object.Register(reinterpret_cast<Ret (T::*)(Params...)>(func));
        }
/*
        template<typename B, typename Ret, typename... Params>
        mock_details::MockedCall<Ret, Params...>& ExpectCall(
            Ret (B::*func)(Params...),
            typename std::enable_if<std::is_base_of<B, T>::value, B>::type* dummy = nullptr)
        {
            return this->object.Register(func);
        }
*/

        void VerifyAll()
        {
            this->object.VerifyExpectations();
        }
    };

} }
