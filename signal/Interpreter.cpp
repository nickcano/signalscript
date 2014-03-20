#include "Interpreter.h"

namespace Signal
{
	Interpreter::Interpreter (Environment& env)
	:
		m_env (env)
	{}

	void Interpreter::execute ()
	{
		std::shared_ptr<Function> func = m_env.find_func ("main");

		if (func.get () == nullptr) {
			throw Error ("Interpreter : main () does not exist.");
		}

		m_frames.push (CallFrame (func));
		m_scopes.push (func->scope ());

		while (m_frames.size () > 0) {
			CallFrame& frame = m_frames.top ();
			Instruction& instruction = (*frame.m_func->code ())[frame.m_address];

			frame.m_address++;

			switch (instruction.m_op)
			{
				case OP_PUSH: m_stack.push (instruction.m_object); break;
				case OP_POP:  m_stack.pop (); break;
				case OP_NIL:  m_stack.push (m_env.obj_nil()); break;

				case OP_CALL:
				{
					std::shared_ptr<Function> call_func = m_env.find_func(instruction.m_object->getString()->text());

					if (call_func.get () == nullptr) {
						throw Error ("Interpreter : function '%s' does not exist.", instruction.m_object->getString()->text());
					}

					m_frames.push (CallFrame(call_func));
					call_func->scope ()->reset ();
					m_scopes.push (std::shared_ptr<Scope> (new Scope (*call_func->scope ())));
				}
				break;

				case OP_MCALL:
				{
					double_t num_args = m_stack.top()->getNumber()->number();
					m_stack.pop ();

					std::shared_ptr<Object> instance = m_stack.top();
					//m_stack.pop();

					Object::Type t = instance.get()->type();
					if (instance.get()->type() != Object::INSTANCE)
						throw Error ("Interpreter : Member call expected class instance.");

					std::shared_ptr<Class> _class = dynamic_cast<Instance*>(instance.get())->_class();
					std::shared_ptr<Function> call_func = _class->find_func(instruction.m_object->getString()->text());
					if (call_func.get () == nullptr) {
						throw Error ("Interpreter : Class '%s' does not define the function '%s'.", _class.get()->name().c_str(), call_func.get()->name().c_str());
					}

					m_frames.push (CallFrame(call_func));
					call_func->scope()->reset();
					call_func->scope()->setParent(_class.get()->scope());
					m_scopes.push(call_func->scope());
				}
				break;

				case OP_RETURN:
				{
					m_frames.pop();
					m_scopes.pop();
				}
				break;

				case OP_ECALL:
				{
					int32_t argCount = m_stack.top()->getNumber()->number();
					m_stack.pop();

					std::vector<std::shared_ptr<Object>> args;
					for (int i = 0; i < argCount; i++)
					{
						args.push_back(m_stack.top());
						m_stack.pop();
					}

					exportedFunction efunc = m_env.findExportedFunction(instruction.m_object->getString()->text());

					if (efunc == nullptr)
						throw Error ("Interpreter : exported function '%s' does not exist.", instruction.m_object->getString()->text());
					
					std::shared_ptr<Object> returnValue = efunc(m_env, args);
					if (returnValue != nullptr)
						m_stack.push(returnValue);
					else
						m_stack.push(m_env.obj_nil());
				}
				break;

				case OP_SET:
				{
					const std::string& name = instruction.m_object->getString()->text();
					m_scopes.top()->set(name, m_stack.top());
				}
				break;

				case OP_DEF:
				{
					const std::string& name = instruction.m_object->getString()->text();
					m_scopes.top()->define(name, m_stack.top());
				}
				break;

				case OP_REF:
				{
					const std::string& name = instruction.m_object->getString()->text();
					std::shared_ptr<Object> arg = m_scopes.top()->find(name);
					m_stack.push (arg);
				}
				break;

				case OP_BR:
				{
					frame.m_address = instruction.m_arg;
				}
				break;

				case OP_BRT:
				{
					if (m_stack.top ()->type () == Object::TRUE) {
						frame.m_address = instruction.m_arg;
					}
					m_stack.pop ();
				}
				break;

				case OP_BRF:
				{
					if (m_stack.top ()->type () == Object::FALSE) {
						frame.m_address = instruction.m_arg;
					}
					m_stack.pop ();
				}
				break;

				case OP_ADD:
				{
					std::shared_ptr<Object> right = m_stack.top ();
					m_stack.pop ();

					std::shared_ptr<Object> left = m_stack.top ();
					m_stack.pop ();
					
					switch (left->type ())
					{
						case Object::NUMBER:
						{
							switch (right->type ())
							{
								case Object::NUMBER:
								{
									m_stack.push (std::shared_ptr<Object> (new Number (dynamic_cast<Number*> (left.get ())->number () + dynamic_cast<Number*> (right.get ())->number ())));
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '+'.");
								}
								break;
							}
						}
						break;

						case Object::STRING:
						{
							switch (right->type ())
							{
								case Object::STRING:
								{
									m_stack.push (std::shared_ptr<Object> (new String (dynamic_cast<String*> (left.get ())->text () + dynamic_cast<String*> (right.get ())->text ())));
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '+'.");
								}
								break;
							}
						}
						break;

						default:
						{
							throw Error ("Interpreter : Invalid arguments to operator '+'.");
						}
						break;
					}
				}
				break;

				case OP_SUB:
				{
					std::shared_ptr<Object> right = m_stack.top ();
					m_stack.pop ();

					std::shared_ptr<Object> left = m_stack.top ();
					m_stack.pop ();
					
					switch (left->type ())
					{
						case Object::NUMBER:
						{
							switch (right->type ())
							{
								case Object::NUMBER:
								{
									m_stack.push (std::shared_ptr<Object> (new Number (dynamic_cast<Number*> (left.get ())->number () - dynamic_cast<Number*> (right.get ())->number ())));
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '-'.");
								}
								break;
							}
						}
						break;

						default:
						{
							throw Error ("Interpreter : Invalid arguments to operator '-'.");
						}
						break;
					}
				}
				break;

				case OP_MUL:
				{
					std::shared_ptr<Object> right = m_stack.top ();
					m_stack.pop ();

					std::shared_ptr<Object> left = m_stack.top ();;
					m_stack.pop ();
					
					switch (left->type ())
					{
						case Object::NUMBER:
						{
							switch (right->type ())
							{
								case Object::NUMBER:
								{
									m_stack.push (std::shared_ptr<Object> (new Number (dynamic_cast<Number*> (left.get ())->number () * dynamic_cast<Number*> (right.get ())->number ())));
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '*'.");
								}
								break;
							}
						}
						break;

						default:
						{
							throw Error ("Interpreter : Invalid arguments to operator '*'.");
						}
						break;
					}
				}
				break;

				case OP_DIV:
				{
					std::shared_ptr<Object> right = m_stack.top ();
					m_stack.pop ();

					std::shared_ptr<Object> left = m_stack.top ();;
					m_stack.pop ();
					
					switch (left->type ())
					{
						case Object::NUMBER:
						{
							switch (right->type ())
							{
								case Object::NUMBER:
								{
									if (dynamic_cast<Number*> (right.get ())->number () == 0) {
										m_stack.push (std::shared_ptr<Object> (new Nil ()));
									} else {
										m_stack.push (std::shared_ptr<Object> (new Number (dynamic_cast<Number*> (left.get ())->number () / dynamic_cast<Number*> (right.get ())->number ())));
									}
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '/'.");
								}
								break;
							}
						}
						break;

						default:
						{
							throw Error ("Interpreter : Invalid arguments to operator '/'.");
						}
						break;
					}
				}
				break;

				case OP_NEG:
				{
					std::shared_ptr<Object> value = m_stack.top ();
					m_stack.pop ();
					
					switch (value->type())
					{
						case Object::NUMBER:
						{
							m_stack.push (std::shared_ptr<Object> (new Number (-dynamic_cast<Number*> (value.get ())->number ())));
						}
						break;

						default:
						{
							throw Error ("Interpreter : Invalid arguments to operator '-'.");
						}
						break;
					}
				}
				break;

				case OP_INC:
				{
					std::shared_ptr<Object> identifier = m_stack.top ();
					switch (identifier->type())
					{
						case Object::NUMBER:
							{
								Number* num = dynamic_cast<Number*>(identifier.get());
								num->set(num->number() + 1);
							}
							break;
						case Object::NIL:
							throw Error ("Interpreter : Attempt to increment an uninitialized variable.");
							break;
						default:
							throw Error ("Interpreter : Attempt to increment invalid value.");
							break;
					}
				}
				break;
				case OP_DEC:
				{
					std::shared_ptr<Object> identifier = m_stack.top ();
					switch (identifier->type())
					{
						case Object::NUMBER:
							{
								Number* num = dynamic_cast<Number*>(identifier.get());
								num->set(num->number() - 1);
							}
							break;
						case Object::NIL:
							throw Error ("Interpreter : Attempt to decrement an uninitialized variable..");
							break;
						default:
							throw Error ("Interpreter : Attempt to decrement invalid value.");
							break;
					}
				}
				break;

				case OP_NOT:
				{
					std::shared_ptr<Object> value = m_stack.top ();
					m_stack.pop ();
					
					switch (value->type ())
					{
						case Object::TRUE:
						{
							m_stack.push (std::shared_ptr<Object> (new False ()));
						}
						break;

						case Object::FALSE:
						{
							m_stack.push (std::shared_ptr<Object> (new True ()));
						}
						break;

						default:
						{
							throw Error ("Interpreter : Invalid arguments to operator '!'.");
						}
						break;
					}
				}
				break;

				case OP_OR:
				{
					std::shared_ptr<Object> right = m_stack.top ();
					m_stack.pop ();

					std::shared_ptr<Object> left = m_stack.top ();;
					m_stack.pop ();
					
					switch (left->type ())
					{
						case Object::TRUE:
						{
							switch (right->type ())
							{
								case Object::TRUE:
								{
									m_stack.push (std::shared_ptr<Object> (new True ()));
								}
								break;

								case Object::FALSE:
								{
									m_stack.push (std::shared_ptr<Object> (new True ()));
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '||'.");
								}
								break;
							}
						}
						break;

						case Object::FALSE:
						{
							switch (right->type ())
							{
								case Object::TRUE:
								{
									m_stack.push (std::shared_ptr<Object> (new True ()));
								}
								break;

								case Object::FALSE:
								{
									m_stack.push (std::shared_ptr<Object> (new False ()));
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '||'.");
								}
								break;
							}
						}
						break;

						default:
						{
							throw Error ("Interpreter : Invalid arguments to operator '||'.");
						}
						break;
					}
				}
				break;

				case OP_AND:
				{
					std::shared_ptr<Object> right = m_stack.top ();
					m_stack.pop ();

					std::shared_ptr<Object> left = m_stack.top ();;
					m_stack.pop ();
					
					switch (left->type ())
					{
						case Object::TRUE:
						{
							switch (right->type ())
							{
								case Object::TRUE:
								{
									m_stack.push (std::shared_ptr<Object> (new True ()));
								}
								break;

								case Object::FALSE:
								{
									m_stack.push (std::shared_ptr<Object> (new False ()));
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '||'.");
								}
								break;
							}
						}
						break;

						case Object::FALSE:
						{
							switch (right->type ())
							{
								case Object::TRUE:
								{
									m_stack.push (std::shared_ptr<Object> (new False ()));
								}
								break;

								case Object::FALSE:
								{
									m_stack.push (std::shared_ptr<Object> (new False ()));
								}
								break;

								default:
								{
									throw Error ("Interpreter : Type mismatch on operator '||'.");
								}
								break;
							}
						}
						break;

						default:
						{
							throw Error ("Interpreter : Invalid arguments to operator '||'.");
						}
						break;
					}
				}
				break;

				case OP_EQEQ:
				{
					std::shared_ptr<Object> right = m_stack.top();
					m_stack.pop();

					std::shared_ptr<Object> left = m_stack.top();
					m_stack.pop();

					if (*left.get() == *right.get())
						m_stack.push(std::shared_ptr<Object>(new True()));
					else
						m_stack.push(std::shared_ptr<Object>(new False()));
				}
				break;

				case OP_NEQ:
				{
					std::shared_ptr<Object> right = m_stack.top ();
					m_stack.pop ();

					std::shared_ptr<Object> left = m_stack.top ();;
					m_stack.pop ();

					if (*left.get() != *right.get())
						m_stack.push(std::shared_ptr<Object> (new True()));
					else
						m_stack.push(std::shared_ptr<Object> (new False()));
				}
				break;

				case OP_LT:
				{
					std::shared_ptr<Object> right = m_stack.top ();
					m_stack.pop ();

					std::shared_ptr<Object> left = m_stack.top ();;
					m_stack.pop ();
					
					if (*left.get() < *right.get())
						m_stack.push(std::shared_ptr<Object> (new True()));
					else
						m_stack.push(std::shared_ptr<Object> (new False()));
				}
				break;

				case OP_GT:
				{
					std::shared_ptr<Object> right = m_stack.top();
					m_stack.pop();

					std::shared_ptr<Object> left = m_stack.top();
					m_stack.pop();
					
					if (*left.get() > *right.get())
						m_stack.push(std::shared_ptr<Object> (new True()));
					else
						m_stack.push(std::shared_ptr<Object> (new False()));
				}
				break;

				case OP_LTE:
				{
					std::shared_ptr<Object> right = m_stack.top();
					m_stack.pop();

					std::shared_ptr<Object> left = m_stack.top();
					m_stack.pop();
					
					if (*left.get() <= *right.get())
						m_stack.push(std::shared_ptr<Object> (new True()));
					else
						m_stack.push(std::shared_ptr<Object> (new False()));
				}
				break;

				case OP_GTE:
				{
					std::shared_ptr<Object> right = m_stack.top();
					m_stack.pop();

					std::shared_ptr<Object> left = m_stack.top();
					m_stack.pop();
					
					if (*left.get() >= *right.get())
						m_stack.push(std::shared_ptr<Object> (new True()));
					else
						m_stack.push(std::shared_ptr<Object> (new False()));
				}
				break;
			}
		}
	}
}