#include <iostream>
#include "google/template.h"

int main() {
  using google::TemplateDictionary;
  using google::TemplateString;

  // -------------------------------------------------
  TemplateDictionary* dict = new TemplateDictionary("section example");
  const char* username = "ken@ken.com"; // GetUsername();   // returns "" for no user
#if 0
  if (username[0] != '\0') {
    TemplateDictionary* sub_dict = dict->AddSectionDictionary("CHANGE_USER");
    sub_dict->SetValue("USERNAME", username);
  } else {
    // don't need to do anything; we want a hidden section, which is the default
  }
#elif 0
  // Instead of the above 'if' statement, we could have done this:
  if (username[0] != '\0') {
    dict->ShowSection("CHANGE_USER");       // adds a single, empty dictionary
    dict->SetValue("USERNAME", username);   // take advantage of inheritence
  } else {
    // don't need to do anything; we want a hidden section, which is the default
  }
#else
  // Or we could have done this:
  dict->SetValueAndShowSection("USERNAME", username, "CHANGE_USER");
#endif

  // Moving on...
  // GetPrevSearches(prev_searches, &num_prev_searches);
  TemplateString prev_searches[] = {"foo", "bar", "baz"};
  int num_prev_searches = 3;
  if (num_prev_searches > 0) {
    for (int i = 0; i < num_prev_searches; ++i) {
      TemplateDictionary* sub_dict = dict->AddSectionDictionary("PREV_SEARCHES");
      sub_dict->SetValue("PREV_SEARCH", prev_searches[i]);
    }
  }

#if 1
  // -------------------------------------------------
  // TemplateDictionary* dict = new TemplateDictionary("include example");
  // GetResults(results, &num_results);
  int num_results = 3;
  for (int i = 0; i < num_results; ++i) {
    TemplateDictionary* sub_dict = dict->AddIncludeDictionary("RESULT_TEMPLATE");
    sub_dict->SetFilename("results.t");
    // FillResultsTemplate(sub_dict, results[i]);
    // sub_dict
  }
#endif

  // -------------------------------------------------
  google::Template* tpl = google::Template::GetTemplate("foo.t", google::STRIP_WHITESPACE);
  // google::TemplateDictionary dict("debug-name");
  // FillDictionary(&dict, ...);
  std::string output;
  bool error_free = tpl->Expand(&output, dict);
  // output now holds the expanded template
  // Expand returns false if the system cannot load any of the template files
  // referenced by the TemplateDictionary.
  std::cout << output << std::endl;
  return 0;
}