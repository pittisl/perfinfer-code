using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NearObjectMove : MonoBehaviour

{
    public float moveSpeed = 0.5f;
    public static bool myDirection = true;
    Rigidbody rb;
    
    public float deltaTime;

    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("I am alive!");
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        //float h = Input.GetAxis("Horizontal");
        //float v = Input.GetAxis("Vertical");

        //Vector3 movement = new Vector3(h, 0.0f, v);
        //rb.MovePosition(transform.position + movement * moveSpeed);
        if (transform.position.x > 2 && myDirection == true)
        {
            myDirection = !myDirection;
        }
        if (transform.position.x < -2 && myDirection == false)
        {
            myDirection = !myDirection;
        }
        if (myDirection)
        {
            transform.position += new Vector3(0.6f, 0.0f, 0.0f) * Time.deltaTime;
        } else
        {
            transform.position += new Vector3(-0.6f, 0.0f, 0.0f) * Time.deltaTime;
        }
        
        // time calculation
        deltaTime += (Time.deltaTime - deltaTime) * 0.1f;
        float fps = 1.0f / deltaTime;
        Debug.Log("Time: " + Time.time * 1000 + " FPS: " + Mathf.Ceil(fps).ToString());
    }
}
