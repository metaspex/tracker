//
// Copyright Metaspex - 2026
// mailto:admin@metaspex.com
//

#include "hx2a/cursor.hpp"
#include "hx2a/cursor_on_key.hpp"

#include "tracker/exception.hpp"
#include "tracker/misc.hpp"
#include "tracker/ontology.hpp"

namespace tracker {

  void code::consult(string ip_address){
    static size_t mc = config::get_number_or(max_consultations_config_name, default_max_consultations);

    if (_consultations.size() >= mc){
      // We have exceeded the maximum size of the log.
      HX2A_LOG(trace) << "The maximum size of the log for code " << get_id() << " has been exceeded. Disregarding the consultation.";
      return;
    }

    _consultations.push_back(make<consultation>(ip_address));
  }

  code_r code::get_from_campaign_and_email(const db::connector& cn, const campaign_r& camp, string email){
    return camp->get_code_from_email(cn, email);
  }

  code_r campaign::get_code_from_email(const db::connector& cn, string email){
    // We need the tracker code which points at this as a campaign and contains exactly the
    // email given in argument.
    cursor c = cursor_on_key<code>(cn->get_index(code::index_by_campaign_email), {.key = {get_id(), email}, .limit = 1});

    while (c.read_next()){
      // We actually need only one match.
      for (const row<code>& r: c.get_rows()){
        return r.get_doc();
      }    
    }

    throw tracker_code_does_not_exist();
  }
  
} // End namespace tracker.
