#include "GUI.h"

using namespace GUI;


//============================================================//
// Button
void Button::draw()
{

}


//============================================================//
// ListBox

int ListBox::getSelectionIndex() const
{
	return this->selected_index;
}

std::string ListBox::getSelection() const
{
	return this->content[this->selected_index];
}

void ListBox::setSelectionIndex(size_t index)
{
	this->selected_index = (index>=content.size()) ? content.size() : index;
}

void ListBox::draw()
{

}
