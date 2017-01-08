//
//  ViewController.swift
//  iOSCube
//
//  Created by Ellis Sparky Hoag on 7/6/16.
//  Copyright © 2016 Ellis Sparky Hoag. All rights reserved.
//

import UIKit
import GLKit

class ViewController: GLKViewController {
    
    @IBOutlet var myCubeView: GLKCubeView!
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        self.view = myCubeView
        self.delegate = myCubeView
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}

