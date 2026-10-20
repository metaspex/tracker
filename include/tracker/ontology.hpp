//
// Copyright Metaspex - 2026
// mailto:admin@metaspex.com
//

#ifndef HX2A_TRACKER_ONTOLOGY_HPP
#define HX2A_TRACKER_ONTOLOGY_HPP

#include "hx2a/db/connector.hpp"
#include "hx2a/element.hpp"
#include "hx2a/exception.hpp"
#include "hx2a/link.hpp"
#include "hx2a/own_list.hpp"
#include "hx2a/root.hpp"
#include "hx2a/slot.hpp"

#include "tracker/misc.hpp"
#include "tracker/tags.hpp"

namespace tracker {

  using namespace hx2a;

  class consultation;
  using consultation_p = ptr<consultation>;
  using consultation_r = rfr<consultation>;

  class code;
  using code_p = ptr<code>;
  using code_r = rfr<code>;

  class campaign;
  using campaign_p = ptr<campaign>;
  using campaign_r = rfr<campaign>;

  // Each consultations keeps the current timestamp and the IP
  // address of the machine where the 1-pixel tracker was consulted.
  class consultation: public element<>
  {
    HX2A_TYPE(consultation, type_tag<"cons">, element,
              (
               (_timestamp, "t"),
               (_ip_address, "i")
               ));
  public:

    consultation(string ip_address):
      _timestamp(*this, time()), // We log the current time.
      _ip_address(*this, std::move(ip_address))
    {
    }

    time_t get_timestamp() const { return _timestamp; }

    const string& get_ip_address() const { return _ip_address; }

  private:

    slot<time_t> _timestamp;
    slot<string> _ip_address;
  };

  // Tracker codes are not purely contained in a campaign, because the
  // campaign to code relationship is a one to many. Instead, codes are
  // whole documents, bearing a strong link to their correponding campaign.
  // So, conveniently, the "code" is actually the automatically-assigned
  // UUID/doc id for the code document.
  // Consultations on the other hand are a one-to-few relationship between
  // code and consultation, so they are contained in the code document.
  // Each consultation generates a new line in the consultations log,
  // up to some defined limit, to protect against robotic abuse.
  class code: public root<>
  {
    HX2A_TYPE(code, type_tag<"code">, root,
              (
               (_campaign, "C"),
               (_email, "e"),
               (_consultations, "c")
               ));
  public:

    using consultations_type = own_list<consultation>;
    using consultations_const_iterator = own_list<consultation>::const_iterator;

    code(const campaign_r& camp, string email):
      _campaign(*this, camp),
      _email(*this, email),
      _consultations(*this)
    {
    }
    
    campaign_r get_campaign() const { return *_campaign; }

    const string& get_email() const { return _email; }

    // Called when the tracker page is consulted. Adds a consultation log entry up to some
    // limit. When the limit is reached we just drop additional consultations.
    // We might want to evolve this into keeping only the last ones, and dropping the
    // older ones, or implement both strategies, with the choice decided with a guideline
    // given in the configuration file.
    void consult(string ip_address);

    size_t consultations_size() const { return _consultations.size(); }
    
    consultations_const_iterator consultations_cbegin() const { return _consultations.cbegin(); }
    
    consultations_const_iterator consultations_cend() const { return _consultations.cend(); }

    static code_r get_from_campaign_and_email(const db::connector&, const campaign_r&, string email);

    // Indexes.

    static constexpr tag_t index_by_campaign_email = config_name<"code_camp_email">;
    
    static constexpr tag_t index_by_email = config_name<"code_email">;
    
  private:

    // The strong link allows to make it so that when a campaign is removed, all the
    // related tracker codes will be removed also.
    link<campaign> _campaign;
    slot<string> _email;
    // The consultations log.
    consultations_type _consultations;
  };

  class campaign: public root<>
  {
    HX2A_TYPE(campaign, type_tag<"camp">, root,
              (
               (_name, "n")
               ));
  public:

    campaign(string name):
      _name(*this, name)
    {
      if (name.empty()){
        throw name_is_missing_or_empty();
      }
    }
    
    const string& get_name() const { return _name; }

    code_r get_code_from_email(const db::connector&, string email);

    // Indexes.

    static constexpr tag_t index_by_name = config_name<"camp_name">;
    
  private:

    slot<string> _name;
  };
  
} // End namespace tracker.

#endif
