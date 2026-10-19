//
// Copyright Metaspex - 2026
// mailto:admin@metaspex.com
//

#ifndef HX2A_TRACKER_PAYLOADS_HPP
#define HX2A_TRACKER_PAYLOADS_HPP

#include "hx2a/element.hpp"
#include "hx2a/own.hpp"
#include "hx2a/own_vector.hpp"
#include "hx2a/projector.hpp"
#include "hx2a/slot.hpp"

#include "tracker/ontology.hpp"
#include "tracker/tags.hpp"

namespace tracker {

  using namespace hx2a;

  class consultation_data;
  using consultation_data_p = ptr<consultation_data>;
  using consultation_data_r = rfr<consultation_data>;
  
  class code_base_data;
  using code_base_data_p = ptr<code_base_data>;
  using code_base_data_r = rfr<code_base_data>;

  class code_data;
  using code_data_p = ptr<code_data>;
  using code_data_r = rfr<code_data>;

  class code_data_with_campaign;
  using code_data_with_campaign_p = ptr<code_data_with_campaign>;
  using code_data_with_campaign_r = rfr<code_data_with_campaign>;

  using code_create_payload = code_base_data;
  using code_create_payload_p = code_base_data_p;
  using code_create_payload_r = code_base_data_r;

  class consult_payload;
  using consult_payload_p = ptr<consult_payload>;
  using consult_payload_r = rfr<consult_payload>;

  class campaign_id_payload;
  using campaign_id_payload_p = ptr<campaign_id_payload>;
  using campaign_id_payload_r = rfr<campaign_id_payload>;
  
  class campaign_data;
  using campaign_data_p = ptr<campaign_data>;
  using campaign_data_r = rfr<campaign_data>;

  using campaign_create_payload = campaign_data;
  using campaign_create_payload_p = campaign_data_p;
  using campaign_create_payload_r = campaign_data_r;

  class campaign_id_and_email_payload;
  using campaign_id_and_email_payload_p = ptr<campaign_id_and_email_payload>;
  using campaign_id_and_email_payload_r = rfr<campaign_id_and_email_payload>;

  class consultation_data: public element<>
  {
    HX2A_TYPE(consultation_data, type_tag<"consultation_data">, element,
              (
               (_timestamp, timestamp_tag),
               (_ip_address, ip_address_tag)
               ));
  public:

    consultation_data(const consultation_r& c):
      _timestamp(*this, c->get_timestamp()),
      _ip_address(*this, c->get_ip_address())
    {
    }

  private:
    
    slot<time_t> _timestamp;
    slot<string> _ip_address;
  };
  
  class code_base_data: public element<>
  {
    HX2A_TYPE(code_base_data, type_tag<"code_base_data">, element,
              (
               (_campaign_id, campaign_id_tag),
               (_email, email_tag)
               ));
  public:

    code_base_data(const code_r& c):
      _campaign_id(*this, c->get_campaign()->get_id()),
      _email(*this, c->get_email())
    {
    }
    
    const doc_id& get_campaign_id() const { return _campaign_id; }

    const string& get_email() const { return _email; }
    
  private:

    slot<doc_id> _campaign_id;
    slot<string> _email;
  };

  class code_data: public code_base_data
  {
    HX2A_TYPE(code_data, type_tag<"code_data">, code_base_data,
              (
               (_consultations, consultations_tag)
               ));
  public:

    code_data(const code_r& c):
      code_base_data(c),
      _consultations(*this)
    {
      // Populating the consultations data.
      _consultations.reserve(c->consultations_size());
      auto i = c->consultations_cbegin();
      auto e = c->consultations_cend();

      while (i != e){
        _consultations.push_back(make<consultation_data>(**i));
        ++i;
      }
    }

  private:

    own_vector<consultation_data> _consultations;
  };

  class code_data_with_campaign: public code_data
  {
    HX2A_TYPE(code_data_with_campaign, type_tag<"code_data_with_campaign">, code_data,
              (
               (_campaign, campaign_tag)
               ));
  public:

    code_data_with_campaign(const code_r& c):
      code_data(c),
      _campaign(*this, make<campaign_data>(c->get_campaign()))
    {
    }

  private:

    own<campaign_data> _campaign;
  };

  class consult_payload: public element<>
  {
    HX2A_TYPE(consult_payload, type_tag<"consult_pld">, element,
              (
               (_tracker_code_id, tracker_code_id_tag),
               (_ip_address, ip_address_tag)
               ));
  public:

    const doc_id& get_tracker_code_id() const { return _tracker_code_id; }

    const string& get_ip_address() const { return _ip_address; }

  private:

    slot<doc_id> _tracker_code_id;
    slot<string> _ip_address;
  };

  class campaign_id_payload: public element<>
  {
    HX2A_TYPE(campaign_id_payload, type_tag<"campaign_id_pld">, element,
              (
               (_campaign_id, campaign_id_tag)
               ));
  public:

    const doc_id& get_campaign_id() const { return _campaign_id; }

  private:

    slot<doc_id> _campaign_id;
  };
  
  class campaign_data: public element<>
  {
    HX2A_TYPE(campaign_data, type_tag<"campaign_data">, element,
              (
               (_name, name_tag)
               ));
  public:

    campaign_data(const campaign_r& c):
      _name(*this, c->get_name())
    {
    }

  private:

    slot<string> _name;
  };

  class campaign_id_and_email_payload: public element<>
  {
    HX2A_TYPE(campaign_id_and_email_payload, type_tag<"campaign_id_and_email_pld">, element,
              (
               (_campaign_id, campaign_id_tag),
               (_email, email_tag)
               ));
  public:

    const doc_id& get_campaign_id() const { return _campaign_id; }

    const string& get_email() const { return _email; }

  private:

    slot<doc_id> _campaign_id;
    slot<string> _email;
  };
  
} // End namespace tracker.

#endif

