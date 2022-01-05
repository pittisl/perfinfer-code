using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovePointLight : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        QualitySettings.vSyncCount = 0;
        Application.targetFrameRate = 300;
    }

    // Update is called once per frame
    void Update()
    {
        GameObject cyl = GameObject.Find("Cylinder");
        transform.position = cyl.transform.position;
        transform.position += new Vector3(0, 2, 0);
    }
}
