#ifndef ConfigurationPanelDesign_HeaderPlusPlus
#define ConfigurationPanelDesign_HeaderPlusPlus

#include <string>
#include <vector>

#include "StringConvert.hpp"

namespace ConfigPanel
{
	inline std::string Escape(const std::string &to, bool URL = false)
	{
		std::string temp;
		temp.reserve(to.size()); //for fast appending
		for(unsigned long i = 0; i < to.size(); ++i)
		{
			switch(to[i])
			{
				case '&':
					if(!URL)
					{
						temp += "&amp;";
					}
					break;
				case '"':
					temp += "&quot;";
					break;
				case '\'':
					temp += "&apos;";
					break;
				case '<':
					temp += "&lt;";
					break;
				case '>':
					temp += "&gt;";
					break;
				default:
					temp += temp[i];
			}
		}
		return(temp);
	}

	struct Element
	{
		enum ElementType
		{
			Input,
			Textarea,
			Label,
			Select,
			Button,
			Break, //linebreak <br />
			String //literally text...
		};
		virtual ElementType Type() const = 0;
		std::string Name, ID;

		virtual std::string HTML() const = 0;

		virtual ~Element(){}
	};

	struct Input: Element
	{
		virtual ElementType Type() const { return(Element::Input); }
		enum InputType
		{
			Button,
			Checkbox,
			File,
			Hidden,
			Image,
			Password,
			Radio,
			Reset,
			Submit,
			Text
		};
		virtual InputType IType() const = 0;
		bool Disabled;
		unsigned long Size;
		std::string Value;

		virtual ~Input(){}
	};
	struct IButton: Input
	{
		virtual InputType IType() const { return(Input::Button); }

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"button\" name=\"" + Escape(Name) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~IButton(){}
	};
	struct ICheckbox: Input
	{
		virtual InputType IType() const { return(Input::Checkbox); }
		bool Checked;

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"checkbox\" name=\"" + Escape(Name) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			if(Checked)
			{
				temp += " checked=\"checked\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~ICheckbox(){}
	};
	struct IFile: Input
	{
		virtual InputType IType() const { return(Input::File); }
		std::string Accept;

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"file\" name=\"" + Escape(Name) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\" accept=\"" + Escape(Accept) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~IFile(){}
	};
	struct IHidden: Input
	{
		virtual InputType IType() const { return(Input::Hidden); }

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"hidden\" name=\"" + Escape(Name) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~IHidden(){}
	};
	struct IImage: Input
	{
		virtual InputType IType() const { return(Input::Image); }
//		std::string Align;
		std::string Alt, Src;

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"image\" name=\"" + Escape(Name) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\" alt=\"" + Escape(Alt) + "\" src=\"" + Escape(Src, true) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~IImage(){}
	};
	struct IPassword: Input
	{
		virtual InputType IType() const { return(Input::Password); }
		unsigned long MaxLength;
		bool ReadOnly;

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"password\" name=\"" + Escape(Name) + "\" maxlength=\"" + ConvTo<std::string>::f(MaxLength) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			if(ReadOnly)
			{
				temp += " readonly=\"readonly\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~IPassword(){}
	};
	struct IRadio: Input
	{
		virtual InputType IType() const { return(Input::Radio); }
		bool Checked;

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"radio\" name=\"" + Escape(Name) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			if(Checked)
			{
				temp += " checked=\"checked\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~IRadio(){}
	};
	struct IReset: Input
	{
		virtual InputType IType() const { return(Input::Reset); }

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"reset\" name=\"" + Escape(Name) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~IReset(){}
	};
	struct ISubmit: Input
	{
		virtual InputType IType() const { return(Input::Submit); }

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"submit\" name=\"" + Escape(Name) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~ISubmit(){}
	};
	struct IText: Input
	{
		virtual InputType IType() const { return(Input::Text); }
		unsigned long MaxLength;
		bool ReadOnly;

		virtual std::string HTML() const
		{
			std::string temp = "<input type=\"text\" name=\"" + Escape(Name) + "\" maxlength=\"" + ConvTo<std::string>::f(MaxLength) + "\" value=\"" + Escape(Value) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			if(ReadOnly)
			{
				temp += " readonly=\"readonly\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\"/>");
		}

		virtual ~IText(){}
	};

	struct Textarea: Element
	{
		virtual ElementType Type() const { return(Element::Textarea); }
		unsigned long Rows, Cols;
		bool Disabled, ReadOnly;
		std::string Text;

		virtual std::string HTML() const
		{
			std::string temp = "<textarea name=\"" + Escape(Name) + "\" rows=\"" + ConvTo<std::string>::f(Rows) + "\" cols=\"" + ConvTo<std::string>::f(Cols) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			if(ReadOnly)
			{
				temp += " readonly=\"readonly\"";
			}
			return(temp + " id=\"" + Escape(ID) + "\">" + Escape(Text) + "</textarea>");
		}

		virtual ~Textarea(){}
	};

	struct Label: Element
	{
		virtual ElementType Type() const { return(Element::Label); }
		std::string For, Text;

		virtual std::string HTML() const
		{
			return("<label name=\"" + Escape(Name) + "\" for=\"" + Escape(For) + "\" id=\"" + Escape(ID) + "\">" + Escape(Text) + "</label>");
		}

		virtual ~Label(){}
	};

	struct Select: Element
	{
		virtual ElementType Type() const { return(Element::Select); }
		bool Disabled, Multiple;
		unsigned long Size;
		struct OptGroup
		{
			std::string Label, ID;
			bool Disabled;
			struct Option
			{
				std::string Label, Value, Text, ID;
				bool Disabled, Selected;
			};
			std::vector<Option> Options;
		};
		std::vector<OptGroup> OptGroups;

		virtual std::string HTML() const
		{
			std::string temp = "<select name=\"" + Escape(Name) + "\" size=\"" + ConvTo<std::string>::f(Size) + "\"";
			if(Multiple)
			{
				temp += " multiple=\"multiple\"";
			}
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			temp += " id=\"" + Escape(ID) + "\">";
			for(unsigned long i = 0; i < OptGroups.size(); ++i)
			{
				if(!OptGroups[i].Label.empty())
				{
					temp += "<optgroup label=\"" + Escape(OptGroups[i].Label) + "\"";
					if(OptGroups[i].Disabled)
					{
						temp += " disabled=\"disabled\"";
					}
					temp += " id=\"" + Escape(OptGroups[i].ID) + "\">";
				}
				for(unsigned long j = 0; j < OptGroups[i].Options.size(); ++j)
				{
					temp += "<option label=\"" + Escape(OptGroups[i].Options[j].Label) + "\"";
					temp += " value=\"" + Escape(OptGroups[i].Options[j].Value) + "\"";
					if(OptGroups[i].Options[j].Disabled)
					{
						temp += " disabled=\"disabled\"";
					}
					if(OptGroups[i].Options[j].Selected)
					{
						temp += " selected=\"selected\"";
					}
					temp += " id=\"" + Escape(OptGroups[i].Options[j].ID) + "\">" + Escape(OptGroups[i].Options[j].Text) + "</option>";
				}
				if(!OptGroups[i].Label.empty())
				{
					temp += "</optgroup>";
				}
			}
			return(temp + "</select>");
		}

		virtual ~Select(){}
	};

	struct Button: Element
	{
		virtual ElementType Type() const { return(Element::Button); }
		bool Disabled;
		std::string BType, Value;
		std::vector<Element*> Contents;

		virtual std::string HTML() const
		{
			std::string temp = "<button name=\"" + Escape(Name) + "\" type=\"" + Escape(BType) + "\" value=\"" + Escape(Value) + "\"";
			if(Disabled)
			{
				temp += " disabled=\"disabled\"";
			}
			temp += " id=\"" + Escape(ID) + "\">";
			for(unsigned long i = 0; i < Contents.size(); ++i)
			{
				temp += Contents[i]->HTML();
			}
			return(temp + "</button>");
		}

		virtual ~Button(){}
	};

	struct Break: Element
	{
		virtual ElementType Type() const { return(Element::Break); }

		virtual std::string HTML() const
		{
			return("<br />");
		}

		virtual ~Break(){}
	};

	struct String: Element
	{
		virtual ElementType Type() const { return(Element::String); }
		std::string Text;

		virtual std::string HTML() const
		{
			return(Escape(Text));
		}

		virtual ~String(){}
	};
}

#endif

