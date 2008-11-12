#include <google/templace.h>

int main() {
  using google::TemplateDictionary;
  TemplateDictionary* dict = new TemplateDictionary("section example");
  const char* username = GetUsername();   // returns "" for no user
  if (username[0] != '\0') {
    TemplateDictionary* sub_dict = dict->AddSectionDictionary("CHANGE_USER");
    sub_dict->SetValue("USERNAME", username);
  } else {
    // don't need to do anything; we want a hidden section, which is the default
  }

  // Instead of the above 'if' statement, we could have done this:
  if (username[0] != '\0') {
    dict->ShowSection("CHANGE_USER");       // adds a single, empty dictionary
    dict->SetValue("USERNAME", username);   // take advantage of inheritence
  } else {
    // don't need to do anything; we want a hidden section, which is the default
  }

  // Or we could have done this:
  dict->SetValueAndShowSection("USERNAME", username, "CHANGE_USER");

  // Moving on...
  GetPrevSearches(prev_searches, &num_prev_searches);
  if (num_prev_searches > 0) {
    for (int i = 0; i < num_prev_searches; ++i) {
      TemplateDictionary* sub_dict = dict->AddSectionDictionary("PREV_SEARCHES");
      sub_dict->SetValue("PREV_SEARCH", prev_searches[i]);
    }
  }

  google::Template* tpl = google::Template::GetTemplate(<filename>, google::STRIP_WHITESPACE);
  google::TemplateDictionary dict("debug-name");
  FillDictionary(&dict, ...);
  string output;
  bool error_free = tpl->Expand(&output, &dict);
  // output now holds the expanded template
  // Expand returns false if the system cannot load any of the template files
  // referenced by the TemplateDictionary.

  return 0;
}