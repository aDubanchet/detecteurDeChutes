
// ajouter un index pipeline 
PUT /_ingest/pipeline/add_timestamp
{
  "description": "Add current timestamp to documents",
  "processors": [
    {
      "set": {
        "field": "timestamp",
        "value": "{{_ingest.timestamp}}"
      }
    }
  ]
}


// créer le timestamp 
PUT /falls/_mapping
{
  "properties": {
    "timestamp": {
      "type": "date"
    }
  }
}

// ajouter un document 
POST /falls/_doc?pipeline=add_timestamp
{
  "patient_id": "12345",
  "location": "chambre",
  "severity": "grave"
}
